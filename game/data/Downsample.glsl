#version 450 core

layout(set = 0, binding = 0) uniform sampler2D highRes;
layout(set = 0, binding = 1, rgba16f) restrict writeonly uniform image2D lowRes;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 imageCoord = ivec2(gl_GlobalInvocationID.xy);
  vec2 o = (vec2(gl_GlobalInvocationID.xy) + 0.5f) / imageSize(lowRes).xy;
  vec4 sum = vec4(0.0);
  sum = texture(highRes, o);
  imageStore(lowRes, ivec2(gl_GlobalInvocationID.xy), sum);
}
