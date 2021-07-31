#version 450 core

layout(location = 0) in vec2 textureCoord;
layout(location = 0) out vec4 color;

layout(constant_id = 0) const int kernelSize = 3;

layout(set = 0, binding = 0) uniform sampler2D src;

layout(push_constant) uniform PushConstants {
  vec4 factor;
  vec2 d;
};

void main() {
  color = vec4(0.0);
  if (kernelSize == 3) {
    color += texture(src, textureCoord + 0.5 * d) * 0.5;
    color += texture(src, textureCoord - 0.5 * d) * 0.5;
  } else if (kernelSize == 5) {
    vec2[2] kernel = vec2[2](
      vec2(0, 0.375),
      vec2(1.2, 0.3125));
    color += texture(src, textureCoord + kernel[1].x * d) * kernel[1].y;
    color += texture(src, textureCoord - kernel[1].x * d) * kernel[1].y;
    color += texture(src, textureCoord) * kernel[0].y;
  } else if (kernelSize == 7) {
    vec2[2] kernel = vec2[2](
      vec2(0.6, 0.390625),
      vec2(2.142857142857143, 0.109375));
    for (int i = 1; i >= 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
  } else if (kernelSize == 9) {
    vec2[3] kernel = vec2[3](
      vec2(0, 0.2734375),
      vec2(1.3333333333333333, 0.328125),
      vec2(3.1111111111111107, 0.03515625));
    for (int i = 2; i > 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
    color += texture(src, textureCoord) * kernel[0].y;
  } else if (kernelSize == 11) {
    vec2[3] kernel = vec2[3](
      vec2(0.625, 0.328125),
      vec2(2.272727272727273, 0.1611328125),
      vec2(4.090909090909091, 0.0107421875));
    for (int i = 2; i >= 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
  } else if (kernelSize == 13) {
    vec2[4] kernel = vec2[4](
      vec2(0, 0.2255859375),
      vec2(1.3846153846153846, 0.314208984375),
      vec2(3.230769230769231, 0.06982421875),
      vec2(5.076923076923077, 0.003173828125));
    for (int i = 3; i > 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
    color += texture(src, textureCoord) * kernel[0].y;
  } else if (kernelSize == 15) {
    vec2[4] kernel = vec2[4](
      vec2(0.6363636363636364, 0.28802490234375),
      vec2(2.333333333333333, 0.18328857421875),
      vec2(4.2, 0.02777099609375),
      vec2(6.066666666666666, 0.00091552734375));
    for (int i = 3; i >= 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
  } else if (kernelSize == 17) {
    vec2[5] kernel = vec2[5](
      vec2(0, 0.196380615234375),
      vec2(1.4117647058823528, 0.2967529296875),
      vec2(3.2941176470588234, 0.09442138671875),
      vec2(5.1764705882352935, 0.0103759765625),
      vec2(7.0588235294117645, 0.0002593994140625));
    for (int i = 4; i > 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
    color += texture(src, textureCoord) * kernel[0].y;
  } else if (kernelSize == 19) {
    vec2[5] kernel = vec2[5](
      vec2(0.6428571428571429, 0.2596588134765625),
      vec2(2.3684210526315788, 0.1922149658203125),
      vec2(4.2631578947368425, 0.0443572998046875),
      vec2(6.157894736842105, 0.003696441650390625),
      vec2(8.052631578947368, 7.2479248046875e-05));
    for (int i = 4; i >= 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
  } else if (kernelSize == 21) {
    vec2[6] kernel = vec2[6](
      vec2(0, 0.17619705200195312),
      vec2(1.4285714285714284, 0.28031349182128906),
      vec2(3.333333333333333, 0.11089324951171875),
      vec2(5.238095238095238, 0.01940631866455078),
      vec2(7.142857142857142, 0.0012683868408203125),
      vec2(9.047619047619047, 2.002716064453125e-05));
    for (int i = 5; i > 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
    color += texture(src, textureCoord) * kernel[0].y;
  } else if (kernelSize == 23) {
    vec2[6] kernel = vec2[6](
      vec2(0.6470588235294118, 0.2382664680480957),
      vec2(2.391304347826087, 0.19483327865600586),
      vec2(4.304347826086957, 0.05844998359680176),
      vec2(6.217391304347826, 0.008022546768188477),
      vec2(8.130434782608695, 0.0004222393035888672),
      vec2(10.043478260869565, 5.4836273193359375e-06));
    for (int i = 5; i >= 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
  } else if (kernelSize == 25) {
    vec2[7] kernel = vec2[7](
      vec2(0, 0.1611802577972412),
      vec2(1.44, 0.26568174362182617),
      vec2(3.36, 0.12177079916000366),
      vec2(5.28, 0.028651952743530273),
      vec2(7.20, 0.003166794776916504),
      vec2(9.12, 0.00013709068298339844),
      vec2(11.04, 1.4901161193847656e-06));
    for (int i = 6; i > 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
    color += texture(src, textureCoord) * kernel[0].y;
  } else if (kernelSize == 27) {
    vec2[7] kernel = vec2[7](
      vec2(0.65, 0.22140145301818848),
      vec2(2.4074074074074074, 0.1942797750234604),
      vec2(4.333333333333333, 0.06983913481235504),
      vec2(6.2592592592592595, 0.013232678174972534),
      vec2(8.185185185185185, 0.0012029707431793213),
      vec2(10.11111111111111, 4.3585896492004395e-05),
      vec2(12.037037037037036, 4.023313522338867e-07));
    for (int i = 6; i >= 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
  } else if (kernelSize == 29) {
    vec2[8] kernel = vec2[8](
      vec2(0, 0.14944598078727722),
      vec2(1.4482758620689655, 0.25281278416514397),
      vec2(3.3793103448275863, 0.1288849487900734),
      vec2(5.310344827586206, 0.03730880096554756),
      vec2(7.241379310344828, 0.005814358592033386),
      vec2(9.172413793103448, 0.0004423968493938446),
      vec2(11.10344827586207, 1.3612210750579834e-05),
      vec2(13.034482758620689, 1.0803341865539551e-07));
    for (int i = 7; i > 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
    color += texture(src, textureCoord) * kernel[0].y;
  } else if (kernelSize == 31) {
    vec2[8] kernel = vec2[8](
      vec2(0.6521739130434783, 0.20766764413565397),
      vec2(2.4193548387096775, 0.1920881448313594),
      vec2(4.354838709677419, 0.0788572384044528),
      vec2(6.290322580645161, 0.018775532953441143),
      vec2(8.225806451612904, 0.002448982559144497),
      vec2(10.161290322580644, 0.00015824194997549057),
      vec2(12.096774193548386, 4.186294972896576e-06),
      vec2(14.03225806451613, 2.8870999813079834e-08));
    for (int i = 7; i >= 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
  } else if (kernelSize == 33) {
    vec2[9] kernel = vec2[9](
      vec2(0, 0.13994993409141898),
      vec2(1.4545454545454546, 0.24148223921656609),
      vec2(3.3939393939393936, 0.13345071114599705),
      vec2(5.333333333333334, 0.04506127908825874),
      vec2(7.2727272727272725, 0.008979602716863155),
      vec2(9.212121212121211, 0.0009946636855602264),
      vec2(11.151515151515152, 5.52590936422348e-05),
      vec2(13.090909090909092, 1.2703239917755127e-06),
      vec2(15.03030303030303, 7.683411240577698e-09));
    for (int i = 8; i > 0; --i) {
      color += texture(src, textureCoord + kernel[i].x * d) * kernel[i].y;
      color += texture(src, textureCoord - kernel[i].x * d) * kernel[i].y;
    }
    color += texture(src, textureCoord) * kernel[0].y;
  }
  color *= factor;
}
