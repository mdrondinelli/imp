#version 450 core

#include "Constants.glsl"

const float STEPS = 10.0f;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(set = 0, binding = 0) uniform sampler2D transmittanceLut;
layout(set = 1, binding = 0, rgba16f) restrict writeonly uniform image2D skyViewLut;

layout(push_constant) uniform PushConstants {
  // 0
  vec3 rayleighScattering;
  // 12
  float mieScattering;
  // 16
  vec3 ozoneAbsorption;
  // 28
  float mieAbsorption;
  // 32
  vec3 lightIrradiance;
  // 44
  float planetRadius;
  // 48
  vec3 lightDirection;
  // 60
  float atmosphereRadius;
  // 64
  float rayleighScaleHeight;
  // 76
  float mieScaleHeight;
  // 80
  float mieG;
  // 84
  float ozoneHeightCenter;
  // 88
  float ozoneHeightRange;
  // 92
  float cameraHeight;
  // 96
};

bool rayPlanet(vec3 o, vec3 d) {
  float planetRadius2 = planetRadius * planetRadius;
  vec3 f = o + vec3(0.0f, planetRadius, 0.0f);
  float b = -dot(f, d);
  float discriminant = planetRadius2 - dot(f + b * d, f + b * d);
  if (discriminant < 0.0f) {
    return false;
  }
  float c = dot(f, f) - planetRadius2;
  float q = b + sign(b) * sqrt(discriminant);
  float t0 = c / q;
  float t1 = q;
  return t0 > 0.0f || t1 > 0.0f;
}

bool rayAtmosphere(vec3 o, vec3 d, out float t0, out float t1) {
  float atmosphereRadius2 = atmosphereRadius * atmosphereRadius;
  vec3 f = o + vec3(0.0f, planetRadius, 0.0f);
  float b = -dot(f, d);
  float discriminant = atmosphereRadius2 - dot(f + b * d, f + b * d);
  if (discriminant < 0.0f) {
    return false;
  }
  float c = dot(f, f) - atmosphereRadius2;
  float q = b + sign(b) * sqrt(discriminant);
  float t0_ = c / q;
  float t1_ = q;
  t0 = min(t0_, t1_);
  t1 = max(t0_, t1_);
  if (t0 > 0.0f) {
    return true;
  } else if (t1 > 0.0f) {
    t0 = 0.0f;
    return true;
  } else {
    return false;
  }
}

float densityR(float h) {
  return exp(-h / rayleighScaleHeight);
}

float densityM(float h) {
  return exp(-h / mieScaleHeight);
}

float densityO(float h) {
  return max(0.0f, 1.0f - abs(h - ozoneHeightCenter) / (0.5f * ozoneHeightRange));
}

vec3 densityVec(float h) {
  return vec3(exp(vec2(-h) / vec2(rayleighScaleHeight, mieScaleHeight)), densityO(h));
}

float phaseR(float mu) {
  float numer = 3.0f * (1.0f + mu * mu);
  float denom = 16.0f * PI;
  return numer / denom;
}

float phaseM(float mu) {
  const float G = 0.8f;
  const float G2 = G * G;
  float numer = 3.0f * (1.0f - G2) * (1.0f + mu * mu);
  float denom = 8.0f * PI * (2.0f + G2) + pow(1.0f + G2 - 2.0f * G * mu, 1.5f);
  return numer / denom;
}

vec3 lookUpTransmittanceRay(vec3 o, vec3 d) {
  float planetRadius2 = planetRadius * planetRadius;
  float atmosphereRadius2 = atmosphereRadius * atmosphereRadius;
  float H2 = atmosphereRadius2 - planetRadius2;
  float H = sqrt(H2);
  o.y += planetRadius;
  float r2 = dot(o, o);
  float r = sqrt(r2);
  float rho2 = r2 - planetRadius2;
  float rho = sqrt(rho2);
  float mu = dot(o / r, d);
  float muH = -sqrt(1.0f - planetRadius2 / r2);
  vec2 params;
  params.x = rho / H;
  params.y = sign(mu - muH) * sqrt(abs(mu - muH) / (1.0f - sign(mu - muH) * muH)) * 0.5f + 0.5f;
  return texture(transmittanceLut, params).xyz;
}

vec3 skyView(float longitude, float mu) {
  vec3 p0 = vec3(0.0f, cameraHeight, 0.0f);
  vec3 v = vec3(
      sqrt(1.0f - mu * mu) * cos(longitude), 
      mu,
      sqrt(1.0f - mu * mu) * sin(longitude));
  float t0, t1;
  if (rayAtmosphere(p0, v, t0, t1)) {
    vec3 pa = p0 + t0 * v;
    vec3 pb = p0 + t1 * v;
    vec3 dp = (pb - pa) / STEPS;
    float ds = (t1 - t0) / STEPS;
    float mu = dot(v, lightDirection);
    mat3 opticalDepthMatrix = mat3(rayleighScattering, vec3(mieScattering + mieAbsorption), ozoneAbsorption);
    vec3 product = vec3(1.0f);
    vec3 constantsR = rayleighScattering * phaseR(mu) * lightIrradiance * ds;
    vec3 constantsM = mieScattering * phaseM(mu) * lightIrradiance * ds;
    vec3 sumR = vec3(0.0f);
    vec3 sumM = vec3(0.0f);
    float ha = length(pa + vec3(0.0f, planetRadius, 0.0f)) - planetRadius;
    vec3 opticalDepthLhs = opticalDepthMatrix * densityVec(ha);
    vec3 transmittanceA = lookUpTransmittanceRay(pa, lightDirection);
    sumR += 0.5f * densityR(ha) * transmittanceA;
    sumM += 0.5f * densityM(ha) * transmittanceA;
    for (float i = 1.0f; i < STEPS; ++i) {
      vec3 p = pa + i * dp;
      float h = length(p + vec3(0.0f, planetRadius, 0.0f)) - planetRadius;
      vec3 opticalDepthRhs = opticalDepthMatrix * densityVec(h);
      product *= exp(-0.5f * (opticalDepthLhs + opticalDepthRhs) * ds);
      opticalDepthLhs = opticalDepthRhs;
      vec3 transmittance = product * lookUpTransmittanceRay(p, lightDirection);
      sumR += densityR(h) * transmittance;
      sumM += densityM(h) * transmittance;
    }
    float hb = atmosphereRadius - planetRadius;
    vec3 opicalDepthRhs = opticalDepthMatrix * densityVec(hb);
    vec3 transmittanceB = product * lookUpTransmittanceRay(pb, lightDirection);
    sumR += 0.5f * densityR(hb) * transmittanceB;
    sumM += 0.5f * densityM(hb) * transmittanceB;
    return constantsR * sumR + constantsM * sumM;
  } else {
    return vec3(0.0f);
  }
}

void main() {
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy);
  vec2 paramsDenom = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy) - 1.0f;
  vec2 params = paramsNumer / paramsDenom;
  params = 2.0f * params - 1.0f;
  float longitude = PI * params.x;
  float r = planetRadius + cameraHeight;
  float r2 = r * r;
  float mu = mix(-sqrt(1.0f - (planetRadius * planetRadius) / r2), sign(params.y), params.y * params.y);
  imageStore(skyViewLut, ivec2(gl_GlobalInvocationID.xy), vec4(skyView(longitude, mu), 0.0f));
}
