#version 450 core


layout(set = 0, binding = 0, rgba16f) restrict uniform image2D renderImage;
layout(set = 0, binding = 1) uniform sampler2D bloomImage;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 renderCoord = ivec2(gl_GlobalInvocationID.xy);
  vec2 bloomCoord =
      (vec2(gl_GlobalInvocationID.xy) + 0.5f) / imageSize(renderImage).xy;
  vec4 renderValue = imageLoad(renderImage, renderCoord);
  vec4 bloomValue = texture(bloomImage, bloomCoord);
  imageStore(renderImage, renderCoord, renderValue + bloomValue);
}
