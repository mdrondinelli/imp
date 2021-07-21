#version 450 core

#include "Constants.glsl"

#define SCENE_SET     0
#define SCENE_BINDING 0
#include "Scene.glsl"

const float STEPS = 40.0f;
float groundRadius2;
float atmosphereRadius2;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(set = 0, binding = 1, rgba16f) restrict writeonly uniform image2D
    transmittanceLut;

float rayAtmosphere(vec2 o, vec2 d) {
  float b = -dot(o, d);
  float discriminant = atmosphereRadius2 - dot(o + b * d, o + b * d);
  float c = dot(o, o) - atmosphereRadius2;
  float q = b + sign(b) * sqrt(discriminant);
  float t0 = c / q;
  float t1 = q;
  return max(t0, t1);
}

float densityR(float h) {
  return min(exp(-h / scene.planet.rayleighScaleHeight), 1.0f);
}

float densityM(float h) {
  return min(exp(-h / scene.planet.mieScaleHeight), 1.0f);
}

float densityO(float h) {
  return max(
      0.0f,
      1.0f - abs(h - scene.planet.ozoneLayerHeight) /
                 (0.5f * scene.planet.ozoneLayerThickness));
}

vec3 transmittance(vec2 pa, vec2 v) {
  float t = rayAtmosphere(pa, v);
  vec2 pb = pa + t * v;
  vec2 dp = (pb - pa) / STEPS;
  float ds = t / STEPS;
  vec3 sumR = vec3(0.0f);
  float sumM = 0.0f;
  vec3 sumO = vec3(0.0f);
  float ha = length(pa) -
             scene.planet.groundRadius;
  sumR += 0.5f * densityR(ha);
  sumM += 0.5f * densityM(ha);
  sumO += 0.5f * densityO(ha);
  for (float i = 1.0f; i < STEPS; ++i) {
    vec2 p = pa + i * dp;
    float h = length(p) -
              scene.planet.groundRadius;
    sumR += densityR(h);
    sumM += densityM(h);
    sumO += densityO(h);
  }
  float hb = scene.planet.atmosphereRadius - scene.planet.groundRadius;
  sumR += 0.5f * densityR(hb);
  sumM += 0.5f * densityM(hb);
  sumO += 0.5f * densityO(hb);
  vec3 constantsR = scene.planet.rayleighScattering * ds;
  float constantsM =
      (scene.planet.mieScattering + scene.planet.mieAbsorption) * ds;
  vec3 constantsO = scene.planet.ozoneAbsorption * ds;
  return exp(-constantsR * sumR - constantsM * sumM - constantsO * sumO);
}

void main() {
  groundRadius2 = scene.planet.groundRadius * scene.planet.groundRadius;
  atmosphereRadius2 =
      scene.planet.atmosphereRadius * scene.planet.atmosphereRadius;
  vec2 paramsNumer = vec2(gl_GlobalInvocationID.xy) + 0.5f;
  vec2 paramsDenom = vec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
  vec2 params = paramsNumer / paramsDenom;
  float r = mix(scene.planet.groundRadius, scene.planet.atmosphereRadius, params.x);
  float mu = 2.0f * params.y - 1.0f;
  vec2 x = vec2(0.0f, r);
  vec2 v = vec2(sqrt(1.0f - mu * mu), mu);
  imageStore(
      transmittanceLut,
      ivec2(gl_GlobalInvocationID.xy),
      vec4(transmittance(x, v), 0.0f));
}
