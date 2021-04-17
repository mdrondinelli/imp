#pragma once

#include <cstring>

#include <array>

#include "../math/vector.h"

namespace imp {
  class static_vertex {
  public:
    static_vertex(vector3f const &position, vector3f const &normal) noexcept {
      auto snorm_normal = to_snorm16(encode_octahedral(normal));
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
      return decode_octahedral(to_f32(snorm));
    }

  private:
    std::array<char, 16> data_;
  };
} // namespace imp