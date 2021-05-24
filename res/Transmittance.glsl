#version 450 core

#include "Constants.glsl"

const float R_GROUND = 6360e3f;
const float R_TOP = 6460e3f;
const vec3 RAYLEIGH_EXTINCTION = vec3(5.802e-6f, 13.558e-6f, 33.1e-6f);
const float MIE_EXTINCTION = 3.996e-6f + 4.40e-6f;
const vec3 OZONE_EXTINCTION = vec3(0.650e-6f, 1.881e-6f, 0.085e-6f);
const float STEPS = 40.0f;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(set = 0, binding = 0, rgba16) restrict writeonly uniform image2D transmittanceLut;

layout(push_constant) uniform PushConstants {
  vec3 rayleighExtinction;
  float mieExtinction;
  vec3 ozoneExtinction;
  float planetRadius;
  float atmosphereRadius;
};

float rayAtmosphere(vec2 o, vec2 d) {
  const float R2 = R_TOP * R_TOP;
  vec2 f = o + vec2(0.0f, R_GROUND);
  float b = -dot(f, d);
  float discriminant = R2 - dot(f + b * d, f + b * d);
  float c = dot(f, f) - R2;
  float q = b + sign(b) * sqrt(discriminant);
  float t0 = c / q;
  float t1 = q;
  return max(t0, t1);
}

float densityR(float h) {
  const float H0 = 8000.0f;
  return min(1.0f, exp(-h / H0));
}

float densityM(float h) {
  const float H0 = 1200.0f;
  return min(1.0f, exp(-h / H0));
}

float densityO(float h) {
  const float CENTER = 25000.0f;
  const float RANGE = 30000.0f;
  return max(0.0f, 1.0f - abs(h - CENTER) / (0.5f * RANGE));
}

vec3 transmittance(float ha, float mu) {
  vec2 pa = vec2(0.0f, ha);
  vec2 v = vec2(sqrt(1.0f - mu * mu), mu);
  float t = rayAtmosphere(pa, v);
  vec2 pb = pa + t * v;
  vec2 dp = (pb - pa) / STEPS;
  float ds = t / STEPS;
  vec3 constantsR = RAYLEIGH_EXTINCTION * ds;
  float constantsM = MIE_EXTINCTION * ds;
  vec3 constantsO = OZONE_EXTINCTION * ds;
  vec3 sumR = vec3(0.0f);
  float sumM = 0.0f;
  vec3 sumO = vec3(0.0f);
  sumR += 0.5f * densityR(ha);
  sumM += 0.5f * densityM(ha);
  sumO += 0.5f * densityO(ha);
  for (float i = 1.0f; i < STEPS; ++i) {
    vec2 p = pa + i * dp;
    float h = length(p + vec2(0.0f, R_GROUND)) - R_GROUND;
    sumR += densityR(h);
    sumM += densityM(h);
    sumO += densityO(h);
  }
  float hb = R_TOP - R_GROUND;
  sumR += 0.5f * densityR(hb);
  sumM += 0.5f * densityM(hb);
  sumO += 0.5f * densityO(hb);
  return exp(-constantsR * sumR - constantsM * sumM - constantsO * sumO);
}

void main() {
  const float RG2 = R_GROUND * R_GROUND;
  const float RT2 = R_TOP * R_TOP;
  const float H = sqrt(RT2 - RG2);
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy);
  vec2 paramsDenom = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy) - 1.0f;
  vec2 params = paramsNumer / paramsDenom;
  float rho = params.x * H;
  float rho2 = rho * rho;
  float r2 = rho2 + RG2;
  float r = sqrt(r2);
  float h = r - R_GROUND;
  params.y = params.y * 2.0 - 1.0f;
  float mu = mix(-sqrt(1.0f - RG2 / r2), sign(params.y), params.y * params.y);
  imageStore(transmittanceLut, ivec2(gl_GlobalInvocationID.xy), vec4(transmittance(h, mu), 0.0f));
}
