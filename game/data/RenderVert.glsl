#version 450 core

#define SCENE_VIEW_SET 0
#define SCENE_VIEW_BINDING 1
#include "SceneView.glsl"

out VertexData {
  layout(location = 0) vec3 viewDirection;
};


vec4 positions[3] = vec4[](
  vec4(-1.0f, -1.0f, 0.0f, 1.0f),
  vec4(-1.0f, 3.0f, 0.0f, 1.0f),
  vec4(3.0f, -1.0f, 0.0f, 1.0f)
);

void main() {
  float u = positions[gl_VertexIndex].x * 0.5f + 0.5f;
  float v = positions[gl_VertexIndex].y * 0.5f + 0.5f;
  viewDirection = mix(
      mix(sceneView.viewRays[0], sceneView.viewRays[1], u),
      mix(sceneView.viewRays[2], sceneView.viewRays[3], u),
      v);
  gl_Position = positions[gl_VertexIndex];
}
