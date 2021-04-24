#version 450 core

out vertex_data {
  layout(location = 0) vec3 eye_direction;
} out_data;

layout(push_constant) uniform push_constants {
  vec3 frustum_corners[4];
  vec3 eye_position;
  vec3 sun_radiance;
  vec3 sun_direction;
  float g;
  float planet_radius;
  float atmosphere_radius;
  uint frame;
};

vec4 clip_positions[3] = vec4[](
  vec4(-1.0f, -1.0f, 0.0f, 1.0f),
  vec4(-1.0f, 3.0f, 0.0f, 1.0f),
  vec4(3.0f, -1.0f, 0.0f, 1.0f)
);

void main() {
  float u = clip_positions[gl_VertexIndex].x * 0.5f + 0.5f;
  float v = clip_positions[gl_VertexIndex].y * 0.5f + 0.5f;
  vec3 point = mix(
      mix(frustum_corners[0], frustum_corners[1], u),
      mix(frustum_corners[2], frustum_corners[3], u),
      v);
  out_data.eye_direction = point - eye_position;
  gl_Position = clip_positions[gl_VertexIndex];
}
