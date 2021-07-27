#include "gtest/gtest.h"

#include <util/Math.h>

TEST(MathTest, octSinglePrecision) {
  auto const EPSILON = 0.001f;
  for (auto i = 0; i < 8; ++i) {
    auto x = Eigen::Vector3f::Random().normalized();
    auto y = imp::decodeOct(imp::encodeOct(x));
    EXPECT_TRUE(((y - x).array().abs() < EPSILON).all());
  }
}

TEST(MathTest, octDoublePrecision) {
  auto const EPSILON = 0.001;
  for (auto i = 0; i < 8; ++i) {
    auto x = Eigen::Vector3d::Random().normalized();
    auto y = imp::decodeOct(imp::encodeOct(x));
    EXPECT_TRUE(((y - x).array().abs() < EPSILON).all());
  }
}