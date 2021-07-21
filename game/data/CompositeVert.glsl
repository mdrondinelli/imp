#version 450 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in uint inVertexIndex;
layout(location = 2) in uint inTextureIndex;

layout(location = 0) out vec2 textureCoord;
layout(location = 1) out uint textureIndex;

vec2 textureCoords[4] = vec2[4](
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f),
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f));

void main() {
  gl_Position = vec4(inPosition, 0.0f, 1.0f);
  textureCoord = textureCoords[inVertexIndex];
  textureIndex = inTextureIndex;
}
