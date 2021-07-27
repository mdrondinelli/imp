#pragma once

#include <cstddef>

#include <type_traits>

namespace imp {
  template<std::size_t Alignment, typename T>
  constexpr T align(T offset) noexcept {
    if constexpr (Alignment == 0) {
      return offset;
    }
    if constexpr (std::is_unsigned_v<T>) {
      auto remainder = offset % Alignment;
      if (remainder == 0) {
        return offset;
      }
      return static_cast<T>(offset + Alignment - remainder);
    } else {
      auto remainder = (offset > 0 ? offset : -offset) % Alignment;
      if (remainder == 0) {
        return offset;
      }
      return static_cast<T>(offset + Alignment - remainder);
    }
  }

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