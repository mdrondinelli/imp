#version 450 core

layout(location = 0) in vec2 textureCoord;
layout(location = 0) out vec4 color;

#include "Constants.glsl"
#include "Intersections.glsl"

#define SCENE_SET     0
#define SCENE_BINDING 0
#include "Scene.glsl"

#define SCENE_VIEW_SET     0
#define SCENE_VIEW_BINDING 1
#include "SceneView.glsl"

layout(set = 0, binding = 2) uniform sampler2D transmittanceLut;
layout(set = 0, binding = 3) uniform sampler2D skyViewLut;

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
  float t2 = abs(latitude) / (0.5 * PI);
  float t = sign(latitude) * sqrt(t2);
  vec2 params;
  params.x = 0.5f * longitude / PI + 0.5f;
  params.y = 0.5 * t + 0.5;
  return texture(skyViewLut, params);
}

vec3 expose(vec3 x) {
  return x * sceneView.exposure;
}

void main() {
  vec3 v = normalize(
      mix(mix(sceneView.viewDirections[0],
              sceneView.viewDirections[1],
              textureCoord.x),
          mix(sceneView.viewDirections[2],
              sceneView.viewDirections[3],
              textureCoord.x),
          textureCoord.y));
  vec4 skyView = loadSkyView(v);
  vec3 skyRadiance = skyView.rgb;
  vec3 sunTransmittance =
      (1.0 - skyView.a) *
      step(scene.sun.cosAngularRadius, dot(v, sceneView.sunDirection)) *
      loadTransmittance(sceneView.altitude, v.z);
  vec3 sunRadiance =
      scene.sun.irradiance / (2.0f * PI * (1.0f - scene.sun.cosAngularRadius));
  vec3 radiance = skyRadiance + sunTransmittance * sunRadiance;
  vec3 forward = sceneView.viewDirections[0] + sceneView.viewDirections[1] +
                 sceneView.viewDirections[2] + sceneView.viewDirections[3];
  vec3 irradiance = radiance * dot(v, normalize(forward));
  color = vec4(expose(irradiance), 1.0);
}
