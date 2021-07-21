vec2 raySphere(vec3 ro, vec3 rd, vec3 ce, float ra) {
  vec3 oc = ro - ce;
  float b = dot(oc, rd);
  float c = dot(oc, oc) - ra * ra;
  float h = b * b - c;
  if (h < 0.0) {
    return vec2(-1.0);
  }
  h = sqrt(h);
  return vec2(-b - h, -b + h);
}

vec2 preciseRaySphere(vec3 ro, vec3 rd, vec3 ce, float ra) {
  vec3 oc = ro - ce;
  float a = dot(rd, rd);
  float b = -dot(oc, rd);
  vec3 temp = oc + b / a * rd;
  float delta = ra * ra - dot(temp, temp);
  if (delta < 0.0f) {
    return vec2(-1.0f);
  }
  float c = dot(oc, oc) - ra * ra;
  float q = b + sign(b) * sqrt(a * delta);
  return vec2(c / q, q / a);
}
