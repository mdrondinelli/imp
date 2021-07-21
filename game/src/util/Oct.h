#pragma once

#include <Eigen/Dense>

namespace imp {
  Eigen::Array2f encodeOct(Eigen::Vector3f const &n_) {
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

  Eigen::Array2d encodeOct(Eigen::Vector3d const &n_) {
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

  Eigen::Vector3f decodeOct(Eigen::Array2f const &f_) {
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

  Eigen::Vector3d decodeOct(Eigen::Array2d const &f_) {
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