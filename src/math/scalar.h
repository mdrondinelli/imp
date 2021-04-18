#pragma once

#include <cmath>

#include <type_traits>

namespace imp {
  template<typename T>
  auto abs(T x) noexcept {
    return std::abs(x);
  }

  template<typename T>
  auto round(T x) noexcept {
    return std::round(x);
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

  template<typename T>
  auto to_unorm8(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::uint8_t>(round(clamp(r, T{0}, T{1}) * T{255}));
  }

  template<typename T>
  auto to_snorm8(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::int8_t>(round(clamp(r, T{-1}, T{1}) * T{127}));
  }

  template<typename T>
  auto to_unorm16(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::uint16_t>(round(clamp(r, T{0}, T{1}) * T{65535}));
  }

  template<typename T>
  auto to_snorm16(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::int16_t>(round(clamp(r, T{-1}, T{1}) * T{32767}));
  }

  template<typename T>
  constexpr auto to_f32(T i) noexcept {
    static_assert(std::is_integral_v<T>);
    if constexpr (std::is_unsigned_v<T>) {
      constexpr auto scale = (std::size_t{1} << (sizeof(T) * 8)) - 1.0f;
      return i / scale;
    } else {
      constexpr auto scale = (std::size_t{1} << (sizeof(T) * 8 - 1)) - 1.0f;
      return max(i / scale, -1.0f);
    }
  }

  template<typename T>
  constexpr auto to_f64(T i) noexcept {
    static_assert(std::is_integral_v<T>);
    if constexpr (std::is_unsigned_v<T>) {
      constexpr auto scale = (std::size_t{1} << (sizeof(T) * 8)) - 1.0;
      return i / scale;
    } else {
      constexpr auto scale = (std::size_t{1} << (sizeof(T) * 8 - 1)) - 1.0;
      return max(i / scale, -1.0);
    }
  }

  // template<typename T>
  // constexpr auto square(T x) noexcept {
  //  return x * x;
  //}

  // template<typename T>
  // constexpr auto cube(T x) noexcept {
  //  return x * x * x;
  //}
} // namespace imp
