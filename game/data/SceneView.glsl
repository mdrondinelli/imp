#ifndef SCENE_VIEW_GLSL
#define SCENE_VIEW_GLSL

#ifndef SCENE_VIEW_SET
#define SCENE_VIEW_SET 0
#endif

#ifndef SCENE_VIEW_BINDING
#define SCENE_VIEW_BINDING 0
#endif

layout(set = SCENE_VIEW_SET, binding = SCENE_VIEW_BINDING) uniform SceneView {
  vec3 skyViewEyeDirections[4];
  vec3 skyViewSunDirection;
  vec2 antiAliasingPositions[4];
  float antiAliasingAlpha;
  float altitude;
  float exposure;
}
sceneView;

#endif
