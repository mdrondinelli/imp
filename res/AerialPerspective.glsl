#version 450 core

#include "Constants.glsl"
#include "Intersections.glsl"

const float STEPS = 10.0f;

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout(set = 0, binding = 0) uniform Atmosphere {
  vec3 rayleighScattering;
  float rayleighScaleHeight;
  float mieScattering;
  float mieAbsorption;
  float mieScaleHeight;
  float mieG;
  vec3 ozoneAbsorption;
  float ozoneLayerHeight;
  float planetRadius;
  float atmosphereRadius;
};

layout(set = 0, binding = 1) uniform Camera {
  vec3 frustumCorners[8];
  float cameraHeight;
};

layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;
layout(set = 0, binding = 3, rgba16f) restrict writeonly uniform image3D aerialPerspectiveLut;

layout(push_constant) uniform PushConstants {
  vec3 lightIrradiance;
  vec3 lightDirection;
};

vec2 rayAtmosphere(vec3 o, vec3 d) {
  return raySphere(o, d, vec3(0.0, -planetRadius, 0.0), atmosphereRadius);
}

float densityR(float h) {
  return min(1.0, exp(h * rayleighHeightFactor));
}

float densityM(float h) {
  return min(1.0, exp(h * mieHeightFactor));
}

float densityO(float h) {
  return max(0.0f, 1.0f - abs(h * ozoneHeightScale + ozoneHeightOffset));
}

vec3 densityRmo(float h) {
  return vec3(exp(vec2(h) * vec2(rayleighScaleHeight, mieScaleHeight)), densityO(h));
}

float phaseR(float mu) {
  float numer = 3.0 * mu * mu + 3.0;
  float denom = 16.0 * PI;
  return numer / denom;
}

float phaseM(float mu) {
  float mieG2 = mieG * mieG;
  float numer = 3.0f * (1.0f - mieG2) * (1.0f + mu * mu);
  float denom = 8.0f * PI * (2.0f + mieG2) + pow(1.0f + mieG2 - 2.0f * mieG * mu, 1.5f);
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
  params.y = 0.5 * sign(mu - muH) * sqrt(abs(mu - muH) / (1.0f - sign(mu - muH) * muH)) + 0.5;
  return texture(transmittanceLut, params).xyz;
}

