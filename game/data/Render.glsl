#version 450 core

#include "Constants.glsl"
#include "Intersections.glsl"
#include "Numeric.glsl"

#define SCENE_SET     0
#define SCENE_BINDING 0
#include "Scene.glsl"

#define SCENE_VIEW_SET     0
#define SCENE_VIEW_BINDING 1
#include "SceneView.glsl"

float groundRadius2 = scene.planet.groundRadius * scene.planet.groundRadius;
float atmosphereRadius2 =
    scene.planet.atmosphereRadius * scene.planet.atmosphereRadius;

layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;
layout(set = 0, binding = 3) uniform sampler2D skyViewLut;
layout(set = 0, binding = 4, rgba16f) restrict writeonly uniform image2D
    renderImage;

float radius(vec3 x) {
  return distance(x, scene.planet.position);
}

float altitude(vec3 x) {
  return radius(x) - scene.planet.groundRadius;
}

vec2 rayGround(vec3 ro, vec3 rd) {
  return raySphere(ro, rd, scene.planet.position, scene.planet.groundRadius);
}

vec2 rayAtmosphere(vec3 ro, vec3 rd) {
  return raySphere(
      ro, rd, scene.planet.position, scene.planet.atmosphereRadius);
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

vec3 loadTransmittance2(vec3 x, vec3 v, vec3 y) {
  x -= scene.planet.position;
  y -= scene.planet.position;
  vec2 r = vec2(length(x), length(y));
  vec2 mu = vec2(dot(v, x), dot(v, y)) / r;
  vec4 params;
  params.xy = (r - vec2(scene.planet.groundRadius)) /
              vec2(scene.planet.atmosphereRadius - scene.planet.groundRadius);
  params.zw = (r.x < r.y ? 0.5f : -0.5f) * mu + 0.5f;
  vec3 fetchp = texture(transmittanceLut, params.xz).rgb;
  vec3 fetchq = texture(transmittanceLut, params.yw).rgb;
  return clamp(r.x < r.y ? fetchp / fetchq : fetchq / fetchp, vec3(0.0f), vec3(1.0f));
}

vec3 loadSkyView(vec3 v) {
  vec3 skyViewNormal =
      cross(sceneView.skyViewTangent, sceneView.skyViewBitangent);
  mat3 skyViewMatrix = transpose(mat3(
      sceneView.skyViewTangent, sceneView.skyViewBitangent, skyViewNormal));
  v = skyViewMatrix * v;
  float longitude = atan(v.y, v.x);
  float latitude = asin(v.z);
  float viewRadius =
      max(radius(sceneView.viewPosition), scene.planet.groundRadius);
  float planetLatitude = -acos(scene.planet.groundRadius / viewRadius);
  float atmosphereLatitude =
      scene.planet.atmosphereRadius < viewRadius
          ? -acos(scene.planet.atmosphereRadius / viewRadius)
          : 0.5f * PI;
  float t2 =
      latitude < planetLatitude
          ? (latitude - planetLatitude) / (-0.5f * PI - planetLatitude)
          : (latitude - planetLatitude) / (atmosphereLatitude - planetLatitude);
  float t = sign(latitude - planetLatitude) * sqrt(t2);
  vec2 params;
  params.x = 0.5f * longitude / PI + 0.5f;
  params.y = 0.5f * t + 0.5f;
  return texture(skyViewLut, params).xyz;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy) + 0.5f;
  vec2 paramsDenom = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
  vec2 params = paramsNumer / paramsDenom;
  vec3 v = normalize(
      mix(mix(sceneView.viewRays[0], sceneView.viewRays[1], params.x),
          mix(sceneView.viewRays[2], sceneView.viewRays[3], params.x),
          params.y));
  vec3 radiance = loadSkyView(v);
  vec2 hits = rayGround(sceneView.viewPosition, v);
  if (hits.x < 0.0f) {
    vec3 sunTransmittance = loadTransmittance(sceneView.viewPosition, v);
    vec3 sunRadiance =
        step(scene.sun.cosAngularRadius, dot(v, scene.sun.direction)) *
        scene.sun.irradiance;
    radiance += sunTransmittance * sunRadiance;
  } else {
    vec3 hitPosition = sceneView.viewPosition + hits.x * v;
    vec3 hitTransmittance = loadTransmittance2(sceneView.viewPosition, v, hitPosition) * loadTransmittance(hitPosition, scene.sun.direction);
    vec3 hitNormal = normalize(hitPosition - scene.planet.position);
    vec3 hitRadiance = scene.planet.albedo * scene.sun.irradiance *
                       clamp(dot(hitNormal, scene.sun.direction), 0.0f, 1.0f);
    radiance += hitTransmittance * hitRadiance;
  }
  imageStore(
      renderImage, ivec2(gl_GlobalInvocationID.xy), vec4(radiance, 1.0f));
}
