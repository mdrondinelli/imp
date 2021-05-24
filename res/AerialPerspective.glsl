#version 450 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout(push_constant) uniform PushConstants {
  vec3 
};
