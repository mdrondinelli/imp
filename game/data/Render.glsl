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

layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;
layout(set = 0, binding = 3) uniform sampler2D skyViewLut;
layout(set = 0, binding = 4, rgba16f) restrict writeonly uniform image2D
    renderImage;

vec3 loadTransmittance(float h, float mu) {
  vec2 params;
  params.x =
      sqrt(h / (scene.planet.atmosphereRadius - scene.planet.groundRadius));
  params.y = 0.5f * sign(mu) * sqrt(abs(mu)) + 0.5f;
  return texture(transmittanceLut, params).rgb;
}

vec4 loadSkyView(vec3 v) {
  float longitude = atan(v.y, v.x);
  float latitude = asin(v.z);
  float t2 =
      (latitude - sceneView.groundLat) /
      ((latitude < sceneView.groundLat ? -0.5f * PI : sceneView.atmosphereLat) -
       sceneView.groundLat);
  float t = sqrt(t2);
  float horizonY =
      (sceneView.groundLat + 0.5f * PI) / (sceneView.atmosphereLat + 0.5f * PI);
  vec2 params;
  params.x = 0.5f * longitude / PI + 0.5f;
  params.y = latitude < sceneView.groundLat ? mix(horizonY, 0.0f, t)
                                            : mix(horizonY, 1.0f, t);
  return texture(skyViewLut, params);
}

float sunVisibility(vec3 v) {
  vec3 dx = 0.25 * (sceneView.viewDirections[1] - sceneView.viewDirections[0]) / imageSize(renderImage).x;
  vec3 dy = 0.25 * (sceneView.viewDirections[2] - sceneView.viewDirections[0]) / imageSize(renderImage).y;
  float vis = 0.0;
  vec3 v0 = normalize(v - dx - dy);
  vis += step(scene.sun.cosAngularRadius, dot(v0, sceneView.sunDirection));
  vec3 v1 = normalize(v - dx + dy);
  vis += step(scene.sun.cosAngularRadius, dot(v1, sceneView.sunDirection));
  vec3 v2 = normalize(v + dx - dy);
  vis += step(scene.sun.cosAngularRadius, dot(v2, sceneView.sunDirection));
  vec3 v3 = normalize(v + dx + dy);
  vis += step(scene.sun.cosAngularRadius, dot(v3, sceneView.sunDirection));
  return 0.25 * vis;
}

vec3 expose(vec3 x) {
  return x * sceneView.exposure;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy) + 0.5f;
  vec2 paramsDenom = vec2(imageSize(renderImage));
  vec2 params = paramsNumer / paramsDenom;
  vec3 v = normalize(mix(
      mix(sceneView.viewDirections[0], sceneView.viewDirections[1], params.x),
      mix(sceneView.viewDirections[2], sceneView.viewDirections[3], params.x),
      params.y));
  vec4 skyView = loadSkyView(v);
  vec3 skyRadiance = skyView.rgb;
  vec3 sunTransmittance =
      skyView.a * loadTransmittance(sceneView.altitude, v.z);
  vec3 sunRadiance =
      scene.sun.irradiance / (2.0f * PI * (1.0f - scene.sun.cosAngularRadius));
  vec3 color = expose(skyRadiance + sunVisibility(v) * sunTransmittance * sunRadiance);
  color = vec4(texture(skyViewLut, textureCoord).rgb, 1.0);
  imageStore(renderImage, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.0f));
}
