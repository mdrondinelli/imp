#version 450 core

in vertex_data {
  layout(location = 0) vec3 eye_direction;
  layout(location = 1) vec2 frustum_uv;
} in_data;

layout(location = 0) out vec4 frag_color;

layout(set = 0, binding = 0) uniform sampler3D scattering_lut;
layout(set = 0, binding = 1) uniform sampler2D optical_depth_lut;

layout(push_constant) uniform push_constants {
  vec3 frustum_corners[4];
  vec3 eye_position;
  vec3 sun_radiance;
  vec3 sun_direction;
  float g;
  float planet_radius;
  float atmosphere_radius;
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

vec3 partial_tonemap(vec3 x) {
  float A = 0.15f;
  float B = 0.50f;
  float C = 0.10f;
  float D = 0.20f;
  float E = 0.02f;
  float F = 0.30f;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 filmic_tonemap(vec3 x) {
  float exposure_bias = 1.0f / 2.0f;
  vec3 W = vec3(11.2f);
  return partial_tonemap(x * exposure_bias) / partial_tonemap(W);
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
#if 0
  float h = eye_position.y + planet_radius;
  float h2 = h * h;
  float rp2 = planet_radius * planet_radius;
  float ra2 = atmosphere_radius * atmosphere_radius;
  float u = sqrt((h2 - rp2) / (ra2 - rp2));
  float v = (1.0f + eye_direction.y) / 2.0f;
  float w = (1.0f - exp(-2.8f * sun_direction.y - 0.8f)) / (1.0f - exp(-3.6f));
  vec4 scattering_lut_value = texture(scattering_lut, vec3(u, v, w));
  float cos_theta = dot(sun_direction, eye_direction);
  vec3 rayleigh_in = sun_radiance * rayleigh_phase(cos_theta) * scattering_lut_value.xyz;
  vec3 mie_in = sun_radiance * mie_phase(cos_theta) * scattering_lut_value.w;
  vec3 scattering_in = rayleigh_in + mie_in;
  float cos_theta = dot(sun_direction, eye_direction);
  vec3 p_a = eye_position + vec3(0.0f, planet_radius, 0.0f);
  float t_planet = ray_planet(p_a, eye_direction);
  float t_atmosphere = ray_atmosphere(p_a, eye_direction);
  float t = min(t_planet, t_atmosphere);
  vec3 p_b = p_a + t * eye_direction;
  vec4 rm_scattering = scattering(p_a, p_b, sun_direction);
  vec3 total_scattering = rm_scattering.xyz + rm_scattering.www;
  vec4 rm_depth = optical_depth(p_a, p_b);
  vec3 total_depth = rm_depth.xyz + rm_depth.www;
  vec3 total_transmittance = exp(-total_depth);
  vec3 incoming_radiance = vec3(0.0f);
  if (t == t_planet) {
    vec3 p_c = p_b + ray_atmosphere(p_b, sun_direction) * sun_direction;
    vec4 rm_depth = optical_depth(p_a, p_b);
    vec3 total_depth = rm_depth.xyz + rm_depth.www;
    vec3 total_transmittance = exp(-total_depth);
    vec3 li = sun_radiance * total_transmittance;
    vec3 brdf = vec3(0.07f) / 3.14f;
    float n_dot_l = max(dot(normalize(p_b), sun_direction), 0.0f);
    incoming_radiance = li * brdf * n_dot_l;
  } else {
    incoming_radiance = sun_radiance * step(0.99999f, cos_theta);
  }
#endif
  vec3 in_radiance = out_radiance * transmittance + in_scattering;
  frag_color = vec4(filmic_tonemap(in_radiance), 1.0f);
}
