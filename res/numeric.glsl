float hash(uvec2 x) {
  uvec2 q = 1103515245u * ((x >> 1u) ^ (x.yx));
  uint n = 1103515245u * (q.x ^ (q.y >> 3u));
  return float(n) * (1.0f / float(0xffffffffu));
}

vec2 hash2(uvec2 x) {
  return vec2(hash(x), hash(uvec2(0xcf019d85u, 0x8664f205u) * x + 1u));
}

uint lcg(uint x) {
  return 0xcf019d85u * x + 1u;
}

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
