#version 450 core

#include "Constants.glsl"

const float STEPS = 10.0f;

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout(set = 0, binding = 0) uniform sampler2D transmittanceLut;
layout(set = 1, binding = 0, rgba16f) restrict writeonly uniform image3D aerialPerspectiveLut;

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
  float cameraNear;
  // 100
  float cameraFar;
  // 104
};
