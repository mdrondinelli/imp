#version 450 core

layout(location = 0) in vec2 textureCoord;
layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform sampler2D src;

void main() {
  vec2 d = 1.0 / textureSize(src, 0);
  color = vec4(0.0);
  color += texture(src, textureCoord + vec2(d.x, d.y));
  color += texture(src, textureCoord + vec2(d.x, -d.y));
  color += texture(src, textureCoord + vec2(-d.x, d.y));
  color += texture(src, textureCoord + vec2(-d.x, -d.y));
  color *= 0.25;
}
