#pragma once

#include <cmath>

#include <type_traits>

#include "Constants.h"

namespace imp {
  template<typename T>
  auto abs(T x) noexcept {
    return std::abs(x);
  }

  template<typename T>
  auto exp(T x) noexcept {
    return std::exp(x);
  }

  template<typename T>
  auto log(T x) noexcept {
    return std::log(x);
  }

  template<typename T, typename U>
  auto pow(T base, U exponent) noexcept {
    return std::pow(base, exponent);
  }

  template<typename T>
  auto sqrt(T x) noexcept {
    return std::sqrt(x);
  }

  template<typename T>
  auto cbrt(T x) noexcept {
    return std::cbrt(x);
  }

  template<typename T>
  auto sin(T x) noexcept {
    return std::sin(x);
  }

  template<typename T>
  auto cos(T x) noexcept {
    return std::cos(x);
  }

  template<typename T>
  auto tan(T x) noexcept {
    return std::tan(x);
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
  constexpr auto toRadians(T x) noexcept {
    return x * Constants<T>::PI / T{180};
  }

  template<typename T>
  constexpr auto toDegrees(T x) noexcept {
    return x * T{180} / Constants<T>::PI;
  }

  template<typename T>
  auto toUnorm8(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::uint8_t>(round(clamp(r, T{0}, T{1}) * T{255}));
  }

  template<typename T>
  auto toSnorm8(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::int8_t>(round(clamp(r, T{-1}, T{1}) * T{127}));
  }

  template<typename T>
  auto toUnorm16(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::uint16_t>(round(clamp(r, T{0}, T{1}) * T{65535}));
  }

  template<typename T>
  auto toSnorm16(T r) noexcept {
    static_assert(std::is_floating_point_v<T>);
    return static_cast<std::int16_t>(round(clamp(r, T{-1}, T{1}) * T{32767}));
  }

  template<typename T>
  constexpr auto toFloat32(T i) noexcept {
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
  constexpr auto toFloat64(T i) noexcept {
    static_assert(std::is_integral_v<T>);
    if constexpr (std::is_unsigned_v<T>) {
      constexpr auto scale = (std::size_t{1} << (sizeof(T) * 8)) - 1.0;
      return i / scale;
    } else {
      constexpr auto scale = (std::size_t{1} << (sizeof(T) * 8 - 1)) - 1.0;
      return max(i / scale, -1.0);
    }
  }
} // namespace imp
