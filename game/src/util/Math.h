#pragma once

#include <Eigen/Dense>

namespace imp {
  template<typename T>
  constexpr auto PI = T(3.14159265358979);

  constexpr float random(std::uint32_t &seed) noexcept {
    seed = 0xcf019d85 * seed + 1;
    return static_cast<float>(seed) / 4294967296.0f;
  }

  inline float lds1(float n) noexcept {
    auto const PHI = 1.6180339887498948482f;
    auto const ALPHA = 1.0f / PHI;
    auto x = ALPHA * n + 0.5f;
    return x - std::floor(x);
  }

  inline Eigen::Vector2f lds2(float n) noexcept {
    using Eigen::Vector2f;
    auto const PHI = 1.32471795724474602596f;
    auto const PHI2 = PHI * PHI;
    auto const ALPHA = Vector2f{1.0f / PHI, 1.0f / PHI2};
    Vector2f x = ALPHA * n + Vector2f{0.5f, 0.5f};
    x(0) -= std::floor(x(0));
    x(1) -= std::floor(x(1));
    return x;
  }

  inline Eigen::Vector3f lds3(float n) noexcept {
    using Eigen::Vector3f;
    auto const PHI = 1.22074408460575947536f;
    auto const PHI2 = PHI * PHI;
    auto const PHI3 = PHI2 * PHI;
    auto const ALPHA = Vector3f{1.0f / PHI, 1.0f / PHI2, 1.0f / PHI3};
    Vector3f x = ALPHA * n + Vector3f{0.5f, 0.5f, 0.5f};
    x(0) -= std::floor(x(0));
    x(1) -= std::floor(x(1));
    x(2) -= std::floor(x(2));
    return x;
  }

  inline float nextLds(float x) noexcept {
    auto const PHI = 1.6180339887498948482f;
    auto const ALPHA = 1.0f / PHI;
    x += ALPHA;
    return x - std::floor(x);
  }

  inline Eigen::Vector2f nextLds(Eigen::Vector2f const &x_) noexcept {
    using Eigen::Vector2f;
    auto const PHI = 1.32471795724474602596f;
    auto const PHI2 = PHI * PHI;
    auto const ALPHA = Eigen::Vector2f{1.0f / PHI, 1.0f / PHI2};
    Vector2f x = x_ + ALPHA;
    x(0) -= std::floor(x(0));
    x(1) -= std::floor(x(1));
    return x;
  }

  inline Eigen::Vector3f nextLds(Eigen::Vector3f const &x_) noexcept {
    using Eigen::Vector3f;
    auto const PHI = 1.22074408460575947536f;
    auto const PHI2 = PHI * PHI;
    auto const PHI3 = PHI2 * PHI;
    auto const ALPHA = Vector3f{1.0f / PHI, 1.0f / PHI2, 1.0f / PHI3};
    Vector3f x = x_ + ALPHA;
    x(0) -= std::floor(x(0));
    x(1) -= std::floor(x(1));
    x(2) -= std::floor(x(2));
    return x;
  }

  inline std::uint32_t
  hash2to1(Eigen::Array<std::uint32_t, 2, 1> const &x) noexcept {
    auto qx = 1103515245u * ((x.x() >> 1u) ^ x.y());
    auto qy = 1103515245u * ((x.y() >> 1u) ^ x.x());
    return 1103515245u * (qx ^ (qy >> 3u));
  }

  inline Eigen::Array<std::uint32_t, 3, 1>
  hash3to3(Eigen::Array<std::uint32_t, 3, 1> const &v_) noexcept {
    auto v = v_;
    v = 1664525u * v + 1013904223u;
    v.x() += v.y() * v.z();
    v.y() += v.z() * v.x();
    v.z() += v.x() * v.y();
    v.x() ^= v.x() >> 16u;
    v.y() ^= v.y() >> 16u;
    v.z() ^= v.z() >> 16u;
    v.x() += v.y() * v.z();
    v.y() += v.z() * v.x();
    v.z() += v.x() * v.y();
    return v;
  }

  inline Eigen::Array<std::uint32_t, 4, 1>
  hash4to4(Eigen::Array<std::uint32_t, 4, 1> const &v_) noexcept {
    auto v = v_;
    v = 1664525u * v + 1013904223u;
    v.x() += v.y() * v.w();
    v.y() += v.z() * v.x();
    v.z() += v.x() * v.y();
    v.w() += v.y() * v.z();
    v.x() ^= v.x() >> 16u;
    v.y() ^= v.y() >> 16u;
    v.z() ^= v.z() >> 16u;
    v.w() ^= v.w() >> 16u;
    v.x() += v.y() * v.w();
    v.y() += v.z() * v.x();
    v.z() += v.x() * v.y();
    v.w() += v.y() * v.z();
    return v;
  }

  inline Eigen::Array2f encodeOct(Eigen::Vector3f const &n_) {
    auto n = Eigen::Array3f{n_};
    n /= std::abs(n.x()) + std::abs(n.y()) + std::abs(n.z());
    if (n.z() < 0.0f) {
      n.head<2>() =
          ((1.0f - n.head<2>().reverse().abs()) *
           (n.head<2>() < 0.0f)
               .select(-Eigen::Array2f::Ones(), Eigen::Array2f::Ones()))
              .eval();
    }
    n.head<2>() *= 0.5f;
    n.head<2>() += 0.5f;
    return n.head<2>();
  }

  inline Eigen::Array2d encodeOct(Eigen::Vector3d const &n_) {
    auto n = Eigen::Array3d{n_};
    n /= std::abs(n.x()) + std::abs(n.y()) + std::abs(n.z());
    if (n.z() < 0.0) {
      n.head<2>() =
          ((1.0 - n.head<2>().reverse().abs()) *
           (n.head<2>() < 0.0)
               .select(-Eigen::Array2d::Ones(), Eigen::Array2d::Ones()))
              .eval();
    }
    n.head<2>() *= 0.5;
    n.head<2>() += 0.5;
    return n.head<2>();
  }

  inline Eigen::Vector3f decodeOct(Eigen::Array2f const &f_) {
    auto f = (2.0f * f_ - 1.0f).eval();
    auto n = Eigen::Array3f{};
    n.head<2>() = f;
    n.z() = 1.0f - std::abs(f.x()) - std::abs(f.y());
    auto t = std::max(-n.z(), 0.0f);
    n.head<2>() +=
        (n.head<2>() < 0.0f)
            .select(Eigen::Array2f::Constant(t), Eigen::Array2f::Constant(-t))
            .eval();
    return n.matrix().normalized();
  }

  inline Eigen::Vector3d decodeOct(Eigen::Array2d const &f_) {
    auto f = (2.0 * f_ - 1.0).eval();
    auto n = Eigen::Array3d{};
    n.head<2>() = f;
    n.z() = 1.0f - std::abs(f.x()) - std::abs(f.y());
    auto t = std::max(-n.z(), 0.0);
    n.head<2>() +=
        (n.head<2>() < 0.0)
            .select(Eigen::Array2d::Constant(t), Eigen::Array2d::Constant(-t))
            .eval();
    return n.matrix().normalized();
  }
} // namespace imp