#version 450 core

#include "Constants.glsl"
#include "Intersections.glsl"

#define SCENE_SET     0
#define SCENE_BINDING 0
#include "Scene.glsl"

#define SCENE_VIEW_SET     0
#define SCENE_VIEW_BINDING 1
#include "SceneView.glsl"

const float STEPS = 10.0f;
float groundRadius2;
float atmosphereRadius2;

layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;
layout(set = 0, binding = 3, rgba16f) restrict writeonly uniform image2D
    skyViewLut;

float radius(vec3 x) {
  return distance(x, scene.planet.position);
}

float altitude(vec3 x) {
  return distance(x, scene.planet.position) - scene.planet.groundRadius;
}

vec2 rayGround(vec3 ro, vec3 rd) {
  return raySphere(ro, rd, scene.planet.position, scene.planet.groundRadius);
}

vec2 rayAtmosphere(vec3 ro, vec3 rd) {
  return raySphere(
      ro, rd, scene.planet.position, scene.planet.atmosphereRadius);
}

vec3 densityVector(float h) {
  vec2 xy = min(
      exp(vec2(-h) /
          vec2(scene.planet.rayleighScaleHeight, scene.planet.mieScaleHeight)),
      vec2(1.0f));
  float z =
      max(1.0f - abs(h - scene.planet.ozoneLayerHeight) /
                     (0.5f * scene.planet.ozoneLayerThickness),
          0.0f);
  return vec3(xy, z);
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

vec3 loadTransmittance(vec3 x, vec3 v) {
  x -= scene.planet.position;
  float r = length(x);
  float mu = dot(v, x) / r;
  vec2 params;
  params.x = (r - scene.planet.groundRadius) /
             (scene.planet.atmosphereRadius - scene.planet.groundRadius);
  params.y = 0.5f * mu + 0.5f;
  return texture(transmittanceLut, params).rgb;
}

vec3 calcSkyView(vec3 x, vec3 v) {
  vec2 hits = rayAtmosphere(x, v);
  float t0 = max(hits.x, 0.0f);
  float t1 = hits.y;
  hits = rayGround(x, v);
  if (hits.x > 0.0f) {
    t1 = min(t1, hits.x);
  } else if (hits.y > 0.0f) {
    t1 = min(t1, hits.x);
  }
  mat3 extinctionMatrix = mat3(
      scene.planet.rayleighScattering,
      vec3(scene.planet.mieScattering + scene.planet.mieAbsorption),
      scene.planet.ozoneAbsorption);
  float ds = (t1 - t0) / STEPS;
  vec3 p0 = x + t0 * v;
  vec3 p1 = x + t1 * v;
  float h = altitude(p0);
  vec3 density = densityVector(h);
  vec3 primaryTransmittanceLhs = extinctionMatrix * density;
  vec3 primaryTransmittanceRhs;
  vec3 primaryTransmittance = vec3(1.0f);
  vec3 transmittance = loadTransmittance(p0, scene.sun.direction);
  vec3 rayleighSum = 0.5f * density.x * transmittance;
  vec3 mieSum = 0.5f * density.y * transmittance;
  for (float i = 1.0f; i < STEPS; ++i) {
    vec3 p = mix(p0, p1, i / STEPS);
    h = altitude(p);
    density = densityVector(h);
    primaryTransmittanceRhs = extinctionMatrix * density;
    primaryTransmittance *=
        exp(-0.5f * (primaryTransmittanceLhs + primaryTransmittanceRhs) * ds);
    primaryTransmittanceLhs = primaryTransmittanceRhs;
    transmittance =
        primaryTransmittance * loadTransmittance(p, scene.sun.direction);
    rayleighSum += density.x * transmittance;
    mieSum += density.y * transmittance;
  }
  h = altitude(p1);
  density = densityVector(h);
  primaryTransmittanceRhs = extinctionMatrix * density;
  primaryTransmittance *=
      exp(-0.5f * (primaryTransmittanceLhs + primaryTransmittanceRhs) * ds);
  transmittance =
      primaryTransmittance * loadTransmittance(p1, scene.sun.direction);
  rayleighSum += 0.5f * density.x * transmittance;
  mieSum += 0.5f * density.y * transmittance;
  float mu = dot(v, scene.sun.direction);
  vec3 rayleighConstants =
      scene.planet.rayleighScattering * phaseR(mu) * scene.sun.irradiance * ds;
  vec3 mieConstants =
      scene.planet.mieScattering * phaseM(mu) * scene.sun.irradiance * ds;
  return rayleighConstants * rayleighSum + mieConstants * mieSum;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  groundRadius2 = scene.planet.groundRadius * scene.planet.groundRadius;
  atmosphereRadius2 =
      scene.planet.atmosphereRadius * scene.planet.atmosphereRadius;
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy) + 0.5f;
  vec2 paramsDenom = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
  vec2 params = paramsNumer / paramsDenom;
  float viewRadius = radius(sceneView.viewPosition);
  float planetLatitude = -acos(scene.planet.groundRadius / viewRadius);
  float atmosphereLatitude =
      scene.planet.atmosphereRadius < viewRadius
          ? -acos(scene.planet.atmosphereRadius / viewRadius)
          : PI * 0.5;
  float t = 2.0f * params.y - 1.0f;
  float t2 = t * t;
  float longitude = (2.0f * params.x - 1.0f) * PI;
  float latitude = t < 0.0f ? mix(planetLatitude, -0.5 * PI, t2)
                            : mix(planetLatitude, atmosphereLatitude, t2);
  float cosLatitude = cos(latitude);
  float sinLatitude = sin(latitude);
  float cosLongitude = cos(longitude);
  float sinLongitude = sin(longitude);
  vec3 v =
      cosLatitude * cosLongitude * sceneView.skyViewTangent +
      cosLatitude * sinLongitude * sceneView.skyViewBitangent +
      sinLatitude * cross(sceneView.skyViewTangent, sceneView.skyViewBitangent);
  imageStore(
      skyViewLut,
      ivec2(gl_GlobalInvocationID.xy),
      vec4(calcSkyView(sceneView.viewPosition, v), 1.0f));
}
