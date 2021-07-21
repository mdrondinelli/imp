#version 450 core

#include "Numeric.glsl"

layout(location = 0) in vec2 textureCoord;
layout(location = 1) flat in uint textureIndex;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D[1024] textures;

layout(push_constant) uniform PushConstants {
  uint ditherSeed;
};

vec3 expose(vec3 x) {
  return x / 10;
}

vec3 tonemap(vec3 x) {
  mat3 inputMat = transpose(mat3(
      vec3(0.59719f, 0.35458f, 0.04823f),
      vec3(0.07600f, 0.90834f, 0.01566f),
      vec3(0.02840f, 0.13383f, 0.83777f)));
  mat3 outputMat = transpose(mat3(
      vec3(1.60475f, -0.53108f, -0.07367f),
      vec3(-0.10208f, 1.10813f, -0.00605f),
      vec3(-0.00327f, -0.07276f, 1.07602f)));
  x = inputMat * x;
  vec3 a = x * (x + 0.0245786f) - 0.000090537f;
  vec3 b = x * (0.983729f * x + 0.4329510f) + 0.238081f;
  x = a / b;
  x = outputMat * x;
  x = clamp(x, vec3(0.0f), vec3(1.0f));
  return x;
}

vec3 gammaCorrect(vec3 x) {
  bvec3 cutoff = lessThan(x, vec3(0.0031308f));
  vec3 hi = 1.055f * pow(x, vec3(1.0f / 2.4f)) - vec3(0.055f);
  vec3 lo = 12.92f * x;
  return mix(hi, lo, cutoff);
}

void main() {
  vec3 radiance = texture(textures[textureIndex], textureCoord).rgb;
  vec3 color = gammaCorrect(tonemap(expose(radiance)));
  uvec3 seed = uvec3(gl_FragCoord.xy, ditherSeed);
  uvec3 hash = pcg3d(seed);
  vec3 rand = hash / float(0xffffffffu);
  rand = 2.0f * rand - 1.0f;
  rand = sign(rand) * (1.0f - sqrt(1.0f - abs(rand)));
  color += 1.0f / 255.0f * rand;
  outColor = vec4(color, 1.0f);
}
