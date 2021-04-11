#pragma once

#include <cstddef>
#include <cstdint>

namespace imp {
  using size_t = std::size_t;
  using ptrdiff_t = std::ptrdiff_t;
  using max_align_t = std::max_align_t;
  using uint8_t = std::uint8_t;
  using uint16_t = std::uint16_t;
  using uint32_t = std::uint32_t;
  using uint64_t = std::uint64_t;
  using uintptr_t = std::uintptr_t;
  using int8_t = std::int8_t;
  using int16_t = std::int16_t;
  using int32_t = std::int32_t;
  using int64_t = std::int64_t;
  using intptr_t = std::intptr_t;

  template <typename T>
  constexpr auto pi() noexcept {
    return static_cast<T>(3.14159265358979323846);
  }
  
  template<typename T>
  constexpr auto square(T a) noexcept {
    return a * a;
  }

  template<typename T>
  constexpr auto cube(T a) noexcept {
    return a * a * a;
  }

  template<typename T>
  constexpr auto min(T a, T b) noexcept {
    return a < b ? a : b;
  }

  template<typename T>
  constexpr auto max(T a, T b) noexcept {
    return a > b ? a : b;
  }

  template<typename T>
  constexpr auto clamp(T x, T a, T b) noexcept {
    return min(max(x, a), b);
  }

  template<typename T>
  constexpr auto lerp(T a, T b, T t) noexcept {
    return (1 - t) * a + t * b;
  }
} // namespace imp
