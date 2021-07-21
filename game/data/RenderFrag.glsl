#version 450 core

#include "Constants.glsl"
#include "Intersections.glsl"

#define SCENE_SET 0
#define SCENE_BINDING 0
#include "Scene.glsl"

#define SCENE_VIEW_SET 0
#define SCENE_VIEW_BINDING 1
#include "SceneView.glsl"

in VertexData {
  layout(location = 0) vec3 viewDirection;
};

layout(location = 0) out vec4 fragColor;

//layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;
layout(set = 0, binding = 2) uniform sampler2D skyViewLut;

vec2 rayPlanet(vec3 ro, vec3 rd) {
  return raySphere(ro, rd, scene.planetPosition, scene.planetRadius);
}

/*
vec3 loadTransmittance(vec3 x, vec3 v) {
  float planetRadius2 = scene.planetRadius * scene.planetRadius;
  float atmosphereRadius2 = scene.atmosphereRadius * scene.atmosphereRadius;
  float H2 = atmosphereRadius2 - planetRadius2;
  float H = sqrt(H2);
  x -= scene.planetPosition;
  float r2 = dot(x, x);
  float r = sqrt(r2);
  float rho2 = r2 - planetRadius2;
  float rho = sqrt(rho2);
  float mu = dot(v, x / r);
  vec2 params;
  params.x = rho / H;
  params.y = 0.5f * mu + 0.5f;
  return texture(transmittanceLut, params).xyz;
}
*/

vec3 loadSkyView(vec3 v) {
  vec3 skyViewNormal = cross(sceneView.skyViewTangent, sceneView.skyViewBitangent);
  mat3 skyViewMatrix = transpose(mat3(
        sceneView.skyViewTangent, 
        sceneView.skyViewBitangent, 
        skyViewNormal));
  v = skyViewMatrix * v;
  float longitude = atan(v.y / v.x);
  float latitude = asin(v.y);
  float viewRadius = distance(scene.planetPosition, sceneView.viewPosition);
  float planetLatitude = -acos(scene.planetRadius / viewRadius);
  float atmosphereLatitude = scene.atmosphereRadius < viewRadius
      ? -acos(scene.atmosphereRadius / viewRadius)
      : 0.5f * PI;
  float t2 = latitude < planetLatitude
      ? (latitude - planetLatitude) / (-0.5f * PI - planetLatitude)
      : (latitude - planetLatitude) / (atmosphereLatitude - planetLatitude);
  float t = sign(planetLatitude - latitude) * sqrt(t2);
  vec2 params;
  params.x = 0.5f * longitude / PI + 0.5f;
  params.y = 0.5f * t + 0.5f;
  return texture(skyViewLut, params).xyz;
}

vec3 expose(vec3 x) {
  return 0.125f * x;
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
  vec3 v = normalize(viewDirection);
  vec3 radiance = rayPlanet(sceneView.viewPosition, v) == vec2(-1.0f) ? loadSkyView(v) : vec3(0.0f);
  /*
  vec3 color = gammaCorrect(tonemap(expose(radiance)));
  uvec3 seed = uvec3(gl_FragCoord.xy, sceneView.seed);
  uvec3 hash = pcg3d(seed);
  vec3 rand = hash / float(0xffffffffu);
  rand = 2.0f * rand - 1.0f;
  rand = sign(rand) * (1.0f - sqrt(1.0f - abs(rand)));
  color += 1.0f / 255.0f * rand;
  */
  fragColor = vec4(radiance, 0.0f);
}
