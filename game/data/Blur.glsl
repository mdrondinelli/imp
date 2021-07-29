#version 450 core

layout(constant_id = 0) const int kernelSize = 3;
layout(set = 0, binding = 0) uniform sampler2D src;
layout(set = 0, binding = 1, rgba16f) restrict writeonly uniform image2D dst;

layout(push_constant) uniform PushConstants {
  int kernelAxis;
};

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  vec2 o = (vec2(gl_GlobalInvocationID.xy) + 0.5) / imageSize(dst).xy;
  vec2 d = vec2(1 - kernelAxis, kernelAxis) / imageSize(dst).xy;
  vec4 sum = vec4(0.0);
  if (kernelSize == 3) {
    sum += texture(src, o + 0.5 * d) * 0.5;
    sum += texture(src, o - 0.5 * d) * 0.5;
  } else if (kernelSize == 5) {
    vec2[2] kernel = vec2[2](
      vec2(0, 0.375),
      vec2(1.2, 0.3125));
    sum += texture(src, o) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
  } else if (kernelSize == 7) {
    vec2[2] kernel = vec2[2](
      vec2(0.6, 0.390625),
      vec2(2.142857142857143, 0.109375));
    sum += texture(src, o + kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o - kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
  } else if (kernelSize == 9) {
    vec2[3] kernel = vec2[3](
      vec2(0, 0.2734375),
      vec2(1.3333333333333333, 0.328125),
      vec2(3.1111111111111107, 0.03515625));
    sum += texture(src, o) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
  } else if (kernelSize == 11) {
    vec2[3] kernel = vec2[3](
      vec2(0.625, 0.328125),
      vec2(2.272727272727273, 0.1611328125),
      vec2(4.090909090909091, 0.0107421875));
    sum += texture(src, o + kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o - kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
  } else if (kernelSize == 13) {
    vec2[4] kernel = vec2[4](
      vec2(0, 0.2255859375),
      vec2(1.3846153846153846, 0.314208984375),
      vec2(3.230769230769231, 0.06982421875),
      vec2(5.076923076923077, 0.003173828125));
    sum += texture(src, o) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o + kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o - kernel[3].x * d) * kernel[3].y;
  } else if (kernelSize == 15) {
    vec2[4] kernel = vec2[4](
      vec2(0.6363636363636364, 0.28802490234375),
      vec2(2.333333333333333, 0.18328857421875),
      vec2(4.2, 0.02777099609375),
      vec2(6.066666666666666, 0.00091552734375));
    sum += texture(src, o + kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o - kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o + kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o - kernel[3].x * d) * kernel[3].y;
  } else if (kernelSize == 17) {
    vec2[5] kernel = vec2[5](
      vec2(0, 0.196380615234375),
      vec2(1.4117647058823528, 0.2967529296875),
      vec2(3.2941176470588234, 0.09442138671875),
      vec2(5.1764705882352935, 0.0103759765625),
      vec2(7.0588235294117645, 0.0002593994140625));
    sum += texture(src, o) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o + kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o - kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o + kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o - kernel[4].x * d) * kernel[4].y;
  } else if (kernelSize == 19) {
    vec2[5] kernel = vec2[5](
      vec2(0.6428571428571429, 0.2596588134765625),
      vec2(2.3684210526315788, 0.1922149658203125),
      vec2(4.2631578947368425, 0.0443572998046875),
      vec2(6.157894736842105, 0.003696441650390625),
      vec2(8.052631578947368, 7.2479248046875e-05));
    sum += texture(src, o + kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o - kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o + kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o - kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o + kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o - kernel[4].x * d) * kernel[4].y;
  } else if (kernelSize == 21) {
    vec2[6] kernel = vec2[6](
      vec2(0, 0.17619705200195312),
      vec2(1.4285714285714284, 0.28031349182128906),
      vec2(3.333333333333333, 0.11089324951171875),
      vec2(5.238095238095238, 0.01940631866455078),
      vec2(7.142857142857142, 0.0012683868408203125),
      vec2(9.047619047619047, 2.002716064453125e-05));
    sum += texture(src, o) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o + kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o - kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o + kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o - kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o + kernel[5].x * d) * kernel[5].y;
    sum += texture(src, o - kernel[5].x * d) * kernel[5].y;
  } else if (kernelSize == 23) {
    vec2[6] kernel = vec2[6](
      vec2(0.6470588235294118, 0.2382664680480957),
      vec2(2.391304347826087, 0.19483327865600586),
      vec2(4.304347826086957, 0.05844998359680176),
      vec2(6.217391304347826, 0.008022546768188477),
      vec2(8.130434782608695, 0.0004222393035888672),
      vec2(10.043478260869565, 5.4836273193359375e-06));
    sum += texture(src, o + kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o - kernel[0].x * d) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o + kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o - kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o + kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o - kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o + kernel[5].x * d) * kernel[5].y;
    sum += texture(src, o - kernel[5].x * d) * kernel[5].y;
  } else if (kernelSize == 25) {
    vec2[7] kernel = vec2[7](
      vec2(0, 0.1611802577972412),
      vec2(1.44, 0.26568174362182617),
      vec2(3.36, 0.12177079916000366),
      vec2(5.28, 0.028651952743530273),
      vec2(7.20, 0.003166794776916504),
      vec2(9.12, 0.00013709068298339844),
      vec2(11.04, 1.4901161193847656e-06));
    sum += texture(src, o) * kernel[0].y;
    sum += texture(src, o + kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o - kernel[1].x * d) * kernel[1].y;
    sum += texture(src, o + kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o - kernel[2].x * d) * kernel[2].y;
    sum += texture(src, o + kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o - kernel[3].x * d) * kernel[3].y;
    sum += texture(src, o + kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o - kernel[4].x * d) * kernel[4].y;
    sum += texture(src, o + kernel[5].x * d) * kernel[5].y;
    sum += texture(src, o - kernel[5].x * d) * kernel[5].y;
    sum += texture(src, o + kernel[6].x * d) * kernel[6].y;
    sum += texture(src, o - kernel[6].x * d) * kernel[6].y;
  }
  imageStore(dst, ivec2(gl_GlobalInvocationID.xy), sum);
}
