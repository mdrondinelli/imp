#version 450 core

out VertexData {
  layout(location = 0) vec3 v;
} outData;

layout(push_constant) uniform push_constants {
  vec3 frustumCorners[4];
  vec3 cameraPosition;
  uint frame;
};

vec4 positions[3] = vec4[](
  vec4(-1.0f, -1.0f, 0.0f, 1.0f),
  vec4(-1.0f, 3.0f, 0.0f, 1.0f),
  vec4(3.0f, -1.0f, 0.0f, 1.0f)
);

void main() {
  float u = positions[gl_VertexIndex].x * 0.5f + 0.5f;
  float v = positions[gl_VertexIndex].y * 0.5f + 0.5f;
  vec3 p = mix(
      mix(frustumCorners[0], frustumCorners[1], u),
      mix(frustumCorners[2], frustumCorners[3], u),
      v);
  outData.v = p - cameraPosition;
  gl_Position = positions[gl_VertexIndex];
}
