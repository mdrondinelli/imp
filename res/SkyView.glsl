#version 450 core

#include "Constants.glsl"

const float R_GROUND = 6360e3f;
const float R_TOP = 6460e3f;
const vec3 RAYLEIGH_SCATTERING = vec3(5.802e-6f, 13.558e-6f, 33.1e-6f);
const float MIE_SCATTERING = 3.996e-6f;
const float STEPS = 20.0f;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(set = 0, binding = 0) uniform sampler2D transmittanceLut;
layout(set = 1, binding = 0, rgba16f) restrict writeonly uniform image2D skyViewLut;

layout(push_constant) uniform PushConstants {
  vec3 lightIrradiance;
  vec3 lightDirection;
  float cameraHeight;
};

bool rayPlanet(vec3 o, vec3 d) {
  const float R2 = R_GROUND * R_GROUND;
  vec3 f = o + vec3(0.0f, R_GROUND, 0.0f);
  float b = -dot(f, d);
  float discriminant = R2 - dot(f + b * d, f + b * d);
  if (discriminant < 0.0f) {
    return false;
  }
  float c = dot(f, f) - R2;
  float q = b + sign(b) * sqrt(discriminant);
  float t0 = c / q;
  float t1 = q;
  return t0 > 0.0f || t1 > 0.0f;
}

bool rayAtmosphere(vec3 o, vec3 d, out float t0, out float t1) {
  const float R2 = R_TOP * R_TOP;
  vec3 f = o + vec3(0.0f, R_GROUND, 0.0f);
  float b = -dot(f, d);
  float discriminant = R2 - dot(f + b * d, f + b * d);
  if (discriminant < 0.0f) {
    return false;
  }
  float c = dot(f, f) - R2;
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
  const float H0 = 8000.0f;
  return exp(-h / H0);
}

float densityM(float h) {
  const float H0 = 1200.0f;
  return exp(-h / H0);
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

vec3 transmittanceRay(vec3 o, vec3 d) {
  const float RG2 = R_GROUND * R_GROUND;
  const float RT2 = R_TOP * R_TOP;
  const float H2 = RT2 - RG2;
  const float H = sqrt(H2);
  o.y += R_GROUND;
  float r2 = dot(o, o);
  float r = sqrt(r2);
  float rho2 = r2 - RG2;
  float rho = sqrt(rho2);
  float mu = dot(o / r, d);
  float muH = -sqrt(1.0f - RG2 / r2);
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
    vec3 constantsR = RAYLEIGH_SCATTERING * phaseR(mu) * lightIrradiance * ds;
    vec3 constantsM = MIE_SCATTERING * phaseM(mu) * lightIrradiance * ds;
    vec3 sumR = vec3(0.0f);
    vec3 sumM = vec3(0.0f);
    vec3 transmittancePaPb = transmittanceRay(pa, v);
    float ha = length(pa + vec3(0.0f, R_GROUND, 0.0f)) - R_GROUND;
    vec3 transmittanceA = transmittanceRay(pa, lightDirection);
    sumR += 0.5f * densityR(ha) * transmittanceA;
    sumM += 0.5f * densityM(ha) * transmittanceA;
    for (float i = 1.0f; i < STEPS; ++i) {
      vec3 p = pa + i * dp;
      float h = length(p + vec3(0.0f, R_GROUND, 0.0f)) - R_GROUND;
      vec3 transmittance = transmittancePaPb / transmittanceRay(p, v) * transmittanceRay(p, lightDirection);
      sumR += densityR(h) * transmittance;
      sumM += densityM(h) * transmittance;
    }
    float hb = R_TOP - R_GROUND;
    vec3 transmittanceB = transmittancePaPb * transmittanceRay(pb, lightDirection);
    sumR += 0.5f * densityR(hb) * transmittanceB;
    sumM += 0.5f * densityM(hb) * transmittanceB;
    return constantsR * sumR + constantsM * sumM;
  } else {
    return vec3(0.0f);
  }
}

void main() {
  const float RG2 = R_GROUND * R_GROUND;
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy);
  vec2 paramsDenom = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy) - 1.0f;
  vec2 params = paramsNumer / paramsDenom;
  params = 2.0f * params - 1.0f;
  float longitude = PI * params.x;
  float r = R_GROUND + cameraHeight;
  float r2 = r * r;
  float mu = mix(-sqrt(1.0f - RG2 / r2), sign(params.y), params.y * params.y);
  imageStore(skyViewLut, ivec2(gl_GlobalInvocationID.xy), vec4(skyView(longitude, mu), 0.0f));
}
