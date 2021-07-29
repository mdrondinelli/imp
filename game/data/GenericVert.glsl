#version 450 core

layout(location = 0) out vec2 textureCoord;

void main() {
  vec2 textureCoords[3] =
      vec2[3](vec2(2.0, 0.0), vec2(0.0, 0.0), vec2(0.0, 2.0));
  vec2 positions[3] =
      vec2[3](vec2(3.0, -1.0), vec2(-1.0, -1.0), vec2(-1.0, 3.0));
  textureCoord = textureCoords[gl_VertexIndex];
  gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
