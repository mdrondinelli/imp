#ifndef NUMERIC_GLSL
#define NUMERIC_GLSL

#include "Constants.glsl"

uint nextUint(inout uint seed) {
  seed = 0xcf019d85u * seed + 1u;
  return seed;
}

int nextInt(inout uint seed) {
  seed = 0xcf019d85u * seed + 1u;
  return int(seed);
}

float nextFloat(inout uint seed) {
  seed = 0xcf019d85u * seed + 1u;
  return float(seed) / 4294967296.0f;
}

// 2 -> 1 hash
uint iqint3(uvec2 x) {
  uvec2 q = 1103515245u * ((x >> 1u) ^ (x.yx));
  return 1103515245u * (q.x ^ (q.y >> 3u));
}

// 1, 2, 3 -> 3 hash
uvec3 pcg3d(uvec3 v) {
  v = v * 1664525u + 1013904223u;
  v.x += v.y * v.z;
  v.y += v.z * v.x;
  v.z += v.x * v.y;
  v ^= v >> 16u;
  v.x += v.y * v.z;
  v.y += v.z * v.x;
  v.z += v.x * v.y;
  return v;
}

// 1, 2, 3, 4 -> 4 hash
uvec4 pcg4d(uvec4 v) {
  v = v * 1664525u + 1013904223u;
  v.x += v.y * v.w;
  v.y += v.z * v.x;
  v.z += v.x * v.y;
  v.w += v.y * v.z;
  v ^= v >> 16u;
  v.x += v.y * v.w;
  v.y += v.z * v.x;
  v.z += v.x * v.y;
  v.w += v.y * v.z;
  return v;
}

// 1 dimensional low discrepancy sequence
float lds1d(float n) {
  const float phi = 1.6180339887498948482f;
  const float alpha = 1.0f / phi;
  return fract(0.5f + n * alpha);
}

// 2 dimensional low discrepancy sequence
vec2 lds2d(float n) {
  const float phi = 1.32471795724474602596f;
  const float phi2 = phi * phi;
  const vec2 alpha = 1.0f / vec2(phi, phi2);
  return fract(0.5f + n * alpha);
}

// 3 dimensional low discrepancy sequence
vec3 lds3d(float n) {
  const float phi = 1.22074408460575947536f;
  const float phi2 = phi * phi;
  const float phi3 = phi2 * phi;
  const vec3 alpha = 1.0f / vec3(phi, phi2, phi3);
  return fract(0.5f + n * alpha);
}

vec3 sampleSphere(vec2 u) {
  u = 2.0f * u - 1.0f;
  float d = 1.0f - (abs(u[0]) + abs(u[1]));
  float r = 1.0f - abs(d);
  float phi = r == 0.0f ? 0.0f : PI / 4.0f * ((abs(u[1]) - abs(u[0])) / r + 1.0f);
  float f = r * sqrt(2.0f - r * r);
  return vec3(f * sign(u[0]) * cos(phi), f * sign(u[1]) * sin(phi), sign(d) * (1.0f - r * r));
}

vec2 encodeOct(vec3 n) {
  n /= abs(n.x) + abs(n.y) + abs(n.z);
  n.xy = n.z >= 0.0f ? n.xy : (1.0f - abs(n.yx)) * mix(vec2(-1.0f), vec2(1.0f), lessThan(n.xy, vec2(0.0f)));
  n.xy = 0.5f * n.xy + 0.5;
  return n.xy;
}

vec3 decodeOct(vec2 f) {
  f = 2.0f * f - 1.0f;
  vec3 n = vec3(f.x, f.y, 1.0f - abs(f.x) - abs(f.y));
  float t = clamp(-n.z, 0.0f, 1.0f);
  n.xy += mix(vec2(-t), vec2(t), lessThan(n.xy, vec2(0.0f)));
  return normalize(n);
}

#endif
