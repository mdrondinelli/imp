#ifndef SCENE_GLSL
#define SCENE_GLSL

#ifndef SCENE_SET
#define SCENE_SET 0
#endif

#ifndef SCENE_BINDING
#define SCENE_BINDING 0
#endif

struct DirectionalLight {
  vec3 irradiance;
  vec3 direction;
  float cosAngularRadius;
};

struct Planet {
  vec3 position;
  float groundRadius;
  float atmosphereRadius;
  vec3 albedo;
  vec3 rayleighScattering;
  float rayleighScaleHeight;
  float mieScattering;
  float mieAbsorption;
  float mieScaleHeight;
  float mieG;
  vec3 ozoneAbsorption;
  float ozoneLayerHeight;
  float ozoneLayerThickness;
};

layout(set = SCENE_SET, binding = SCENE_BINDING) uniform Scene {
  Planet planet;
  DirectionalLight sun;
}
scene;

#endif
