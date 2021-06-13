#pragma once

#include <cstring>

#include <Eigen/Dense>

namespace imp {
  class StaticVertex {
  public:
    StaticVertex(Eigen::Vector3f const &position, Eigen::Vector3f const &normal) noexcept {
      /*auto snorm_normal = toSnorm16(encodeOctahedral(normal));
      std::memcpy(&data_[0], &position, 12);
      std::memcpy(&data_[12], &snorm_normal, 4);*/
    }

    /*Vector3f getPosition() const noexcept {
      auto p = Vector3f{};
      std::memcpy(&p, &data_[0], 12);
      return p;
    }

    Vector3f getNormal() const noexcept {
      auto snorm = Vector<2, int16_t>{};
      std::memcpy(&snorm, &data_[12], 4);
      return decodeOctahedral(toFloat32(snorm));
    }*/

  private:
    alignas(alignof(std::max_align_t)) std::array<char, 16> data_;
  };
} // namespace imp