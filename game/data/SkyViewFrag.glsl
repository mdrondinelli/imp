#version 450 core

layout(location = 0) in vec2 textureCoord;
layout(location = 0) out vec4 skyView;

#include "Constants.glsl"
#include "Intersections.glsl"

#define SCENE_SET     0
#define SCENE_BINDING 0
#include "Scene.glsl"

#define SCENE_VIEW_SET     0
#define SCENE_VIEW_BINDING 1
#include "SceneView.glsl"

const float STEPS = 30.0f;
const float INV_STEPS = 1.0f / STEPS;

layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;

vec3 planetPosition = vec3(0.0f, 0.0f, -scene.planet.groundRadius);

float radius(vec3 x) {
  return distance(x, planetPosition);
}

float phaseR(float mu) {
  float numer = 3.0f * (1.0f + mu * mu);
  float denom = 16.0f * PI;
  return numer / denom;
}

float phaseM(float mu) {
  float g = scene.planet.mieG;
  float g2 = g * g;
  float numer = 3.0f * (1.0f - g2) * (1.0f + mu * mu);
  float denom = 8.0f * PI * (2.0f + g2) * pow(1.0f + g2 - 2.0f * g * mu, 1.5f);
  return numer / denom;
}

vec2 rayGround(vec3 ro, vec3 rd) {
  return raySphere(ro, rd, planetPosition, scene.planet.groundRadius);
}

vec2 rayAtmosphere(vec3 ro, vec3 rd) {
  return raySphere(ro, rd, planetPosition, scene.planet.atmosphereRadius);
}

vec3 rayScene(vec3 ro, vec3 rd) {
  vec2 t = rayAtmosphere(ro, rd);
  if (t == vec2(-1.0)) {
    return vec3(-1.0);
  }
  vec2 t2 = rayGround(ro, rd);
  t.x = max(t.x, 0.0);
  t.y = t2.x > 0.0 ? min(t2.x, t.y) : t.y;
  float z = t2.x > 0.0 ? 1.0 : 0.0;
  return vec3(t, z);
}

vec2 calcDensity(float h) {
  return min(
      exp(vec2(-h) /
          vec2(scene.planet.rayleighScaleHeight, scene.planet.mieScaleHeight)),
      vec2(1.0f));
}

vec3 loadTransmittance(float h, float mu) {
  vec2 params;
  params.x =
      sqrt(h / (scene.planet.atmosphereRadius - scene.planet.groundRadius));
  params.y = 0.5f * sign(mu) * sqrt(abs(mu)) + 0.5f;
  return texture(transmittanceLut, params).rgb;
}

vec4 calcSkyView(vec3 x, vec3 v) {
  vec3 t = rayScene(x, v);
  if (t == vec3(-1.0)) {
    return vec4(0.0);
  }
  float ds = (t.y - t.x) * INV_STEPS;
  vec3 p0 = x + t.x * v;
  vec3 p1 = x + t.y * v;
  vec3 p = p0 - planetPosition;
  float r = length(p);
  float h = r - scene.planet.groundRadius;
  vec3 n = p / r;
  vec3 eyeTransmittanceDir = t.z != 0.0 ? -v : v;
  vec3 eyeTransmittance = loadTransmittance(h, dot(n, eyeTransmittanceDir));
  vec3 sunTransmittance = loadTransmittance(h, dot(n, sceneView.skyViewSunDirection));
  vec3 transmittance;
  vec2 density = calcDensity(h);
  vec3 rayleighSum = 0.5f * density.r * sunTransmittance;
  vec3 mieSum = 0.5f * density.g * sunTransmittance;
  for (float i = 1.0f; i < STEPS; i += 1.0f) {
    p = mix(p0, p1, i * INV_STEPS) - planetPosition;
    r = length(p);
    h = r - scene.planet.groundRadius;
    n = p / r;
    sunTransmittance = loadTransmittance(h, dot(n, sceneView.skyViewSunDirection));
    transmittance = loadTransmittance(h, dot(n, eyeTransmittanceDir));
    transmittance = t.z == 0.0 ? sunTransmittance * eyeTransmittance / transmittance
                        : sunTransmittance * transmittance / eyeTransmittance;
    density = calcDensity(h);
    rayleighSum += density.r * transmittance;
    mieSum += density.g * transmittance;
  }
  p = p1 - planetPosition;
  r = length(p);
  h = r - scene.planet.groundRadius;
  n = p / r;
  sunTransmittance = loadTransmittance(h, dot(n, sceneView.skyViewSunDirection));
  transmittance = loadTransmittance(h, dot(n, eyeTransmittanceDir));
  transmittance = t.z == 0.0 ? sunTransmittance * eyeTransmittance / transmittance
                      : sunTransmittance * transmittance / eyeTransmittance;
  density = calcDensity(h);
  rayleighSum += 0.5f * density.r * transmittance;
  mieSum += 0.5f * density.g * transmittance;
  float mu = dot(v, sceneView.skyViewSunDirection);
  rayleighSum *= scene.planet.rayleighScattering * phaseR(mu);
  mieSum *= scene.planet.mieScattering * phaseM(mu);
  vec3 indirect = (rayleighSum + mieSum) * ds;
  vec3 direct = t.z == 0.0 ? vec3(0.0f)
                    : transmittance * scene.planet.albedo * INV_PI *
                          max(dot(n, sceneView.skyViewSunDirection), 0.0f);
  vec3 total = (indirect + direct) * scene.sun.irradiance;
  return vec4(total, t.z);
}

void main() {
  float t = 2.0 * textureCoord.y - 1.0;
  float t2 = t * t;
  float longitude = 2.0f * PI * textureCoord.x - PI;
  float latitude = 0.5 * PI * sign(t) * t2;
  float cosLat = cos(latitude);
  float sinLat = sin(latitude);
  float cosLong = cos(longitude);
  float sinLong = sin(longitude);
  vec3 x = vec3(0.0f, 0.0f, sceneView.altitude);
  vec3 v = vec3(cosLat * cosLong, cosLat * sinLong, sinLat);
  skyView = calcSkyView(x, v);
}
