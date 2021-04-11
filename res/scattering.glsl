#version 450 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout(set = 0, binding = 0, rgba16f) restrict writeonly uniform image3D lut;

layout(push_constant) uniform push_constants {
  vec4 scattering_coefficients;
  float planet_radius;
  float atmosphere_radius;
  int integration_steps;
};

vec3 ray_atmosphere(vec3 o, vec3 d) {
  float tca = dot(-o, d);
  float d2 = dot(o, o) - tca * tca;
  float thc = sqrt(atmosphere_radius * atmosphere_radius - d2);
  return o + (tca + thc) * d;
}

vec2 rm_density(float h) {
  return exp(-vec2(h) / vec2(8000.0f, 1200.0f));
}

vec4 rm_depth(vec3 p_a, vec3 p_b) {
  vec2 sum = vec2(0.0f);
  for (int i = 0; i < integration_steps; ++i) {
    vec3 p = mix(p_a, p_b, float(i) / integration_steps);
    float h = length(p) - planet_radius;
    sum += rm_density(h) * (i == 0 ? 0.5f : 1.0f);
  }
  vec4 extinction_coefficients = vec4(vec3(1.0f), 1.1f) * scattering_coefficients;
  vec4 constants = extinction_coefficients * distance(p_a, p_b) / integration_steps;
  return constants * sum.xxxy;
}

vec4 rm_scattering(vec3 p_a, vec3 p_b, vec3 l) {
  vec4 sum = vec4(0.0f);
  for (int i = 0; i < integration_steps; ++i) {
    vec3 p = mix(p_a, p_b, float(i) / integration_steps);
    vec3 p_c = ray_atmosphere(p, l);
    float h = length(p) - planet_radius;
    vec2 density = rm_density(h);
    vec4 depth = rm_depth(p_a, p) + rm_depth(p, p_c);
    sum += density.xxxy * exp(-depth) * (i == 0 ? 0.5f : 1.0f);
  }
  vec4 constants = scattering_coefficients / 12.56f * distance(p_a, p_b) / integration_steps;
  return constants * sum;
}

void main() {
  vec3 params_numer = vec3(gl_GlobalInvocationID) + 0.5f;
  vec3 params_denom = vec3(gl_NumWorkGroups * gl_WorkGroupSize);
  vec3 params = params_numer / params_denom;
  float rp2 = planet_radius * planet_radius;
  float ra2 = atmosphere_radius * atmosphere_radius;
  float h = sqrt(params.x * params.x * (ra2 - rp2) + rp2);
  float cos_phi = params.y * 2.0f - 1.0f;
  float sin_phi = sqrt(1.0f - cos_phi * cos_phi);
  float cos_delta = (log(1.0f - (1.0f - exp(-3.6)) * params.z) + 0.8f) / -2.8f;
  float sin_delta = sqrt(1.0f - cos_delta * cos_delta);
  vec3 p_a = vec3(0.0f, h, 0.0f);
  vec3 v = vec3(sin_phi, cos_phi, 0.0f);
  vec3 p_b = ray_atmosphere(p_a, v);
  vec3 l = vec3(sin_delta, cos_delta, 0.0f);
  vec4 scattering = rm_scattering(p_a, p_b, l);
  imageStore(lut, ivec3(gl_GlobalInvocationID), scattering);
}
