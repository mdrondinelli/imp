#version 450 core

#include "Constants.glsl"
#include "Numeric.glsl"

const float R_GROUND = 4360e3f;

in VertexData {
  layout(location = 0) vec3 v;
} inData;

layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform sampler2D transmittanceLut;
layout(set = 1, binding = 0) uniform sampler2D skyViewLut;

layout(push_constant) uniform push_constants {
  vec3 frustumCorners[4];
  vec3 cameraPosition;
  uint frame;
};

bool rayPlanet(vec3 o, vec3 d) {
  const float R = 6360e3f;
  const float R2 = R * R;
  vec3 f = o + vec3(0.0f, R, 0.0f);
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

vec3 tonemap(vec3 x) {
  mat3 inputMat = mat3(
      vec3(0.59719f, 0.35458f, 0.04823f),
      vec3(0.07600f, 0.90834f, 0.01566f),
      vec3(0.02840f, 0.13383f, 0.83777f));
  mat3 outputMat = mat3(
      vec3(1.60475f, -0.53108f, -0.07367f),
      vec3(-0.10208f, 1.10813f, -0.00605f),
      vec3(-0.00327f, -0.07276f, 1.07602f));
  x = transpose(inputMat) * x;
  vec3 a = x * (x + 0.0245786f) - 0.000090537f;
  vec3 b = x * (0.983729f * x + 0.4329510f) + 0.238081f;
  x = a / b;
  x = transpose(outputMat) * x;
  x = clamp(x, vec3(0.0f), vec3(1.0f));
  return x;
}

vec3 gammaCorrect(vec3 x) {
  bvec3 cutoff = lessThan(x, vec3(0.0031308f));
  vec3 hi = 1.055f * pow(x, vec3(1.0f / 2.4f)) - vec3(0.055f);
  vec3 lo = 12.92f * x;
  return mix(hi, lo, cutoff);
}

vec3 transmittance(vec3 v) {
  const float RG2 = 6360e3f * 6360e3f;
  const float RT2 = 6460e3f * 6460e3f;
  const float H = sqrt(RT2 - RG2);
  float r = cameraPosition.y + 6360e3f;
  float r2 = r * r;
  float rho = sqrt(r2 - RG2);
  float mu = v.y;
  vec2 params = vec2(rho / H, 0.5f * mu + 0.5f);
  return texture(transmittanceLut, params).xyz;
}

vec3 skyView(vec3 v) {
  const float RG2 = R_GROUND * R_GROUND;
  float longitude = acos(v.x) * sign(v.z);
  float r = cameraPosition.y + R_GROUND;
  float r2 = r * r;
  float mu = v.y;
  float muH = -sqrt(1.0f - RG2 / r2);
  vec2 params;
  params.x = longitude / PI * 0.5f + 0.5f;
  params.y = sign(mu - muH) * sqrt(abs(mu - muH) / (1.0f - sign(mu - muH) * muH)) * 0.5f + 0.5f;
  return texture(skyViewLut, params).xyz;
}

void main() {
  vec3 v = normalize(inData.v);
  vec3 inRadiance = rayPlanet(cameraPosition, v) ? vec3(0.0f) : skyView(v);
  vec3 linearColor = tonemap(inRadiance / 8.0f);
  vec3 srgbColor = gammaCorrect(linearColor);
  uvec3 seed = uvec3(gl_FragCoord.xy, frame);
  uvec3 hash = pcg3d(seed);
  vec3 rand = hash / float(0xffffffffu);
  rand = rand * 2.0f - 1.0f;
  rand = sign(rand) * (1.0f - sqrt(1.0f - abs(rand)));
  srgbColor += 1.0f / 255.0f * rand;
  fragColor = vec4(srgbColor, 1.0f);
}
