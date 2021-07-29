#version 450 core

layout(location = 0) in vec2 textureCoord;
layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform sampler2D src;

void main() {
  color = texture(src, textureCoord);
}
