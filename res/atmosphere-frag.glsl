#version 450 core

#include "numeric.glsl"

in vertex_data {
  layout(location = 0) vec3 eye_direction;
} in_data;

layout(location = 0) out vec4 frag_color;

layout(set = 0, binding = 0) uniform sampler2D optical_depth_lut;
layout(set = 0, binding = 1) uniform sampler3D scattering_lut;

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

bool ray_atmosphere(vec3 o, vec3 d, out float t0, out float t1) {
  vec3 l = -o;
  float tca = dot(l, d);
  float d2 = dot(l, l) - tca * tca;
  if (d2 > atmosphere_radius * atmosphere_radius) {
    return false;
  }
  float thc = sqrt(atmosphere_radius * atmosphere_radius - d2);
  if (tca + thc <= 0.0f) {
    return false;
  }
  t0 = max(tca - thc, 0.0f);
  t1 = tca + thc;
  return true;
}

bool ray_planet(vec3 o, vec3 d, out float t) {
  vec3 l = -o;
  float tca = dot(l, d);
  float d2 = dot(l, l) - tca * tca;
  if (d2 > planet_radius * planet_radius) {
    return false;
  }
  float thc = sqrt(planet_radius * planet_radius - d2);
  float t0 = tca - thc;
  float t1 = tca + thc;
  if (t0 > 0.0f) {
    t = t0;
    return true;
  }
  if (t1 > 0.0f) {
    t = t1;
    return true;
  }
  return false;
}

float rayleigh_phase(float cos_theta) {
  // return 0.75f * (1.0f + cos_theta * cos_theta);
  return 0.8f * (1.4f + 0.5f * cos_theta);
}

float mie_phase(float cos_theta) {
  float numer = 3.0f;
  numer *= 1.0f - g * g;
  numer *= 1.0f + cos_theta * cos_theta;
  float denom = 2.0f;
  denom *= 2.0f + g * g;
  denom *= pow(1.0f + g * g - 2.0f * g * cos_theta, 1.5f);
  return numer / denom;
}

float tonemap(float x) {
  x *= 0.6f;
  float a = 2.51f;
  float b = 0.03f;
  float c = 2.43f;
  float d = 0.59f;
  float e = 0.14f;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

vec3 compute_in_scattering(vec3 p_a, vec3 p_b) {
  float rp2 = planet_radius * planet_radius;
  float ra2 = atmosphere_radius * atmosphere_radius;
  float h_a = length(p_a);
  float h_b = length(p_b);
  float u_a = sqrt(max((h_a * h_a - rp2) / (ra2 - rp2), 0.0f));
  float u_b = sqrt(max((h_b * h_b - rp2) / (ra2 - rp2), 0.0f));
  vec3 eye_direction = normalize(p_b - p_a);
  vec3 zenith_a = p_a / h_a;
  vec3 zenith_b = p_b / h_b;
  float v_a = dot(zenith_a, h_a < h_b ? eye_direction : -eye_direction) * 0.5f + 0.5f;
  float v_b = dot(zenith_b, h_a < h_b ? eye_direction : -eye_direction) * 0.5f + 0.5f;
  float w_a = (1.0f - exp(-2.8f * dot(zenith_a, sun_direction) - 0.8f)) / (1.0f - exp(-3.6f));
  float w_b = (1.0f - exp(-2.8f * dot(zenith_b, sun_direction) - 0.8f)) / (1.0f - exp(-3.6f));
  vec4 lut_value = abs(texture(scattering_lut, vec3(u_a, v_a, w_a)) - texture(scattering_lut, vec3(u_b, v_b, w_b)));
  float cos_theta = dot(eye_direction, sun_direction);
  vec3 rayleigh = rayleigh_phase(cos_theta) * lut_value.xyz;
  float mie = mie_phase(cos_theta) * lut_value.w;
  return sun_radiance * (rayleigh + mie);
}

vec3 compute_transmittance_ray(vec3 o, vec3 d) {
  float rp2 = planet_radius * planet_radius;
  float ra2 = atmosphere_radius * atmosphere_radius;
  float h = length(o);
  float u = sqrt(max((h * h - rp2) / (ra2 - rp2), 0.0f));
  vec3 zenith = o / h;
  float v = dot(zenith, d) * 0.5f + 0.5f;
  vec4 lut_value = texture(optical_depth_lut, vec2(u, v));
  return exp(-lut_value.xyz - lut_value.www);
}

vec3 compute_transmittance_segment(vec3 p_a, vec3 p_b) {
  float rp2 = planet_radius * planet_radius;
  float ra2 = atmosphere_radius * atmosphere_radius;
  float h_a = length(p_a);
  float h_b = length(p_b);
  float u_a = sqrt(max((h_a * h_a - rp2) / (ra2 - rp2), 0.0f));
  float u_b = sqrt(max((h_b * h_b - rp2) / (ra2 - rp2), 0.0f));
  vec3 eye_direction = normalize(h_a < h_b ? p_b - p_a : p_a - p_b);
  vec3 zenith_a = p_a / h_a;
  vec3 zenith_b = p_b / h_b;
  float v_a = dot(zenith_a, eye_direction) * 0.5f + 0.5f;
  float v_b = dot(zenith_b, eye_direction) * 0.5f + 0.5f;
  vec4 lut_value = abs(texture(optical_depth_lut, vec2(u_a, v_a)) - texture(optical_depth_lut, vec2(u_b, v_b)));
  return exp(-lut_value.xyz - lut_value.www);
 }

void main() {
  vec3 eye_direction = normalize(in_data.eye_direction);
  vec3 in_scattering = vec3(0.0f);
  vec3 transmittance = vec3(0.0f);
  vec3 out_radiance = vec3(0.0f);
  vec3 p_a = eye_position + vec3(0.0f, planet_radius, 0.0f);
  float t0, t1;
  if (ray_atmosphere(p_a, eye_direction, t0, t1)) {
    float tplanet;
    if (ray_planet(p_a, eye_direction, tplanet)) {
      t1 = tplanet;
      vec3 p_b = p_a + t1 * eye_direction;
      vec3 li = sun_radiance * compute_transmittance_ray(p_b, sun_direction);
      vec3 brdf = vec3(0.1f) / 3.14159f;
      vec3 n = normalize(p_b);
      float n_dot_l = max(dot(n, sun_direction), 0.0f);
      out_radiance = li * brdf * n_dot_l;
    } else {
      out_radiance = sun_radiance * step(0.99999f, dot(eye_direction, sun_direction));
    }
    vec3 p_b = p_a + t1 * eye_direction;
    p_a = p_a + t0 * eye_direction;
    in_scattering = compute_in_scattering(p_a, p_b);
    transmittance = compute_transmittance_segment(p_a, p_b); 
  }
  vec3 in_radiance = out_radiance * transmittance + in_scattering;
  in_radiance *= 0.25f;
  float in_luminance = dot(in_radiance, vec3(0.2126f, 0.7152f, 0.0722f));
  vec3 linear_color = in_radiance / in_luminance * tonemap(in_luminance);
  vec3 srgb_color = pow(linear_color, vec3(1.0f / 2.2f));
  uvec3 seed = uvec3(gl_FragCoord.xy, frame);
  uvec3 hash = pcg3d(seed);
  vec3 rand = hash / float(0xffffffffu);
  rand = rand * 2.0f - 1.0f;
  rand = sign(rand) * (1.0f - sqrt(1.0f - abs(rand)));
  srgb_color += 2.0f / 255.0f * rand;
  frag_color = vec4(srgb_color, 1.0f);
}
