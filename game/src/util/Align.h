#pragma once

#include <cstddef>

#include <type_traits>

namespace imp {
  template<typename T>
  constexpr T align(T alignment, T offset) noexcept {
    if (alignment == 0) {
      return offset;
    }
    if constexpr (std::is_unsigned_v<T>) {
      auto remainder = offset % alignment;
      if (remainder == 0) {
        return offset;
      }
      return static_cast<T>(offset + alignment - remainder);
    } else {
      auto remainder = (offset > 0 ? offset : -offset) % alignment;
      if (remainder == 0) {
        return offset;
      }
      return static_cast<T>(offset + alignment - remainder);
    }
  }
} // namespace imp