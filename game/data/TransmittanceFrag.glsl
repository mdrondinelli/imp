#version 450 core

layout(location = 0) in vec2 textureCoord;
layout(location = 0) out vec4 transmittance;

#include "Constants.glsl"
#include "Numeric.glsl"

#define SCENE_SET     0
#define SCENE_BINDING 0
#include "Scene.glsl"

const float STEPS = 40.0f;
const float INV_STEPS = 1.0f / STEPS;

float rayAtmosphere(vec2 o, vec2 d) {
  o.y += scene.planet.groundRadius;
  float b = -dot(o, d);
  float discriminant =
      scene.planet.atmosphereRadius * scene.planet.atmosphereRadius -
      dot(o + b * d, o + b * d);
  float c =
      dot(o, o) - scene.planet.atmosphereRadius * scene.planet.atmosphereRadius;
  float q = b + sign(b) * sqrt(discriminant);
  float t0 = c / q;
  float t1 = q;
  return max(t0, t1);
}

float altitude(vec2 x) {
  return length(x + vec2(0.0f, scene.planet.groundRadius)) -
         scene.planet.groundRadius;
}

vec3 density(float h) {
  vec3 d;
  d.xy = min(
      exp(vec2(-h) /
          vec2(scene.planet.rayleighScaleHeight, scene.planet.mieScaleHeight)),
      1.0f);
  d.z =
      max(1.0f - abs(h - scene.planet.ozoneLayerHeight) /
                     (0.5f * scene.planet.ozoneLayerThickness),
          0.0f);
  return d;
}

vec3 calcTransmittance(vec2 x, vec2 v) {
  float t = rayAtmosphere(x, v);
  float ds = t * INV_STEPS;
  vec2 dx = v * ds;
  vec3 densitySum = 0.5f * density(altitude(x));
  for (float i = 1.0f; i < STEPS; i += 1.0f) {
    x += dx;
    densitySum += density(altitude(x));
  }
  x += dx;
  densitySum += 0.5f * density(altitude(x));
  mat3 extinctionMat = mat3(
      scene.planet.rayleighScattering,
      vec3(scene.planet.mieScattering + scene.planet.mieAbsorption),
      scene.planet.ozoneAbsorption);
  vec3 extinctionVec = extinctionMat * densitySum * ds;
  return exp(-extinctionVec);
}

void main() {
  float h = (scene.planet.atmosphereRadius - scene.planet.groundRadius) *
            textureCoord.x * textureCoord.x;
  float mu = 2.0f * textureCoord.y - 1.0f;
  mu = sign(mu) * mu * mu;
  vec2 x = vec2(0.0f, h);
  vec2 v = vec2(sqrt(1.0f - mu * mu), mu);
  transmittance = vec4(calcTransmittance(x, v), 0.0);
}
