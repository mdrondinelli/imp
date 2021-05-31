#version 450 core

#include "Constants.glsl"

const float STEPS = 40.0f;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(set = 0, binding = 0, rgba16) restrict writeonly uniform image2D transmittanceLut;

layout(push_constant) uniform PushConstants {
  // 0
  vec3 rayleighExtinction;
  // 12
  float rayleighScaleHeight;
  // 16
  vec3 ozoneExtinction;
  // 28
  float ozoneHeightCenter;
  // 32
  float ozoneHeightRange;
  // 36
  float mieExtinction;
  // 40
  float mieScaleHeight;
  // 44
  float planetRadius;
  // 48
  float atmosphereRadius;
  // 52
};

float rayAtmosphere(vec2 o, vec2 d) {
  float atmosphereRadius2 = atmosphereRadius * atmosphereRadius;
  vec2 f = o + vec2(0.0f, planetRadius);
  float b = -dot(f, d);
  float discriminant = atmosphereRadius2 - dot(f + b * d, f + b * d);
  float c = dot(f, f) - atmosphereRadius2;
  float q = b + sign(b) * sqrt(discriminant);
  float t0 = c / q;
  float t1 = q;
  return max(t0, t1);
}

float densityR(float h) {
  return min(1.0f, exp(-h / rayleighScaleHeight));
}

float densityM(float h) {
  return min(1.0f, exp(-h / mieScaleHeight));
}

float densityO(float h) {
  return max(0.0f, 1.0f - abs(h - ozoneHeightCenter) / (0.5f * ozoneHeightRange));
}

vec3 transmittance(float ha, float mu) {
  vec2 pa = vec2(0.0f, ha);
  vec2 v = vec2(sqrt(1.0f - mu * mu), mu);
  float t = rayAtmosphere(pa, v);
  vec2 pb = pa + t * v;
  vec2 dp = (pb - pa) / STEPS;
  float ds = t / STEPS;
  vec3 constantsR = rayleighExtinction * ds;
  float constantsM = mieExtinction * ds;
  vec3 constantsO = ozoneExtinction * ds;
  vec3 sumR = vec3(0.0f);
  float sumM = 0.0f;
  vec3 sumO = vec3(0.0f);
  sumR += 0.5f * densityR(ha);
  sumM += 0.5f * densityM(ha);
  sumO += 0.5f * densityO(ha);
  for (float i = 1.0f; i < STEPS; ++i) {
    vec2 p = pa + i * dp;
    float h = length(p + vec2(0.0f, planetRadius)) - planetRadius;
    sumR += densityR(h);
    sumM += densityM(h);
    sumO += densityO(h);
  }
  float hb = atmosphereRadius - planetRadius;
  sumR += 0.5f * densityR(hb);
  sumM += 0.5f * densityM(hb);
  sumO += 0.5f * densityO(hb);
  return exp(-constantsR * sumR - constantsM * sumM - constantsO * sumO);
}

void main() {
  const float planetRadius2 = planetRadius * planetRadius;
  const float atmosphereRadius2 = atmosphereRadius * atmosphereRadius;
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy);
  vec2 paramsDenom = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy) - 1.0f;
  vec2 params = paramsNumer / paramsDenom;
  float rho = params.x * sqrt(atmosphereRadius2 - planetRadius2);
  float rho2 = rho * rho;
  float r2 = rho2 + planetRadius2;
  float r = sqrt(r2);
  float h = r - planetRadius;
  float u = params.y * 2.0f - 1.0f;
  float mu = mix(-sqrt(1.0f - planetRadius2 / r2), sign(u), u * u);
  imageStore(transmittanceLut, ivec2(gl_GlobalInvocationID.xy), vec4(transmittance(h, mu), 0.0f));
}
