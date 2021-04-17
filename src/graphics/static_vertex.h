#pragma once

#include <cstring>

#include <array>

#include "../math/vector.h"

namespace imp {
  class static_vertex {
  public:
    static_vertex(vector3f const &position, vector3f const &normal) noexcept {
      auto projected_normal =
          make_vector(normal[0], normal[1]) /
          (abs(normal[0]) + abs(normal[1]) + abs(normal[2]));
      auto encoded_normal =
          normal[2] <= 0.0f
              ? (1.0f -
                 make_vector(
                     abs(projected_normal[1]), abs(projected_normal[0]))) *
                    make_vector(
                        projected_normal[0] >= 0.0f ? 1.0f : -1.0f,
                        projected_normal[1] >= 0.0f ? 1.0f : -1.0f)
              : projected_normal;
      auto snorm_normal = to_snorm16(encoded_normal);
      std::memcpy(&data_[0], &position, 12);
      std::memcpy(&data_[12], &snorm_normal, 4);
    }

    vector3f position() const noexcept {
      auto p = vector3f{};
      std::memcpy(&p, &data_[0], 12);
      return p;
    }

    vector3f normal() const noexcept {
      auto snorm = vector<2, int16_t>{};
      std::memcpy(&snorm, &data_[12], 4);
      auto e = to_f32(snorm);
      auto n = make_vector(e[0], e[1], 1.0f - abs(e[0]) - abs(e[1]));
      if (n[2] < 0.0f) {
        n[0] = (1.0f - abs(e[1])) * (e[0] >= 0.0f ? 1.0f : -1.0f);
        n[1] = (1.0f - abs(e[0])) * (e[1] >= 0.0f ? 1.0f : -1.0f);
      }
      return normalize(n);
    }

  private:
    std::array<char, 16> data_;
  };
} // namespace imp