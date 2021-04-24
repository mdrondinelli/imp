#pragma once

#include <array>
#include <ostream>

#include "Scalar.h"

namespace imp {
  /**
   * @brief An N-element vector.
   */
  template<std::size_t N, typename T>
  class Vector {
  public:
    using Scalar = T;

    constexpr Vector() noexcept: elements_{} {}

    constexpr Vector(T x) noexcept: elements_{} {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = x;
    }

    constexpr Vector &operator=(T x) noexcept {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = x;
    }

    template<typename U>
    constexpr Vector(Vector<N, U> const &rhs) noexcept: elements_{} {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = static_cast<T>(rhs[i]);
    }

    template<typename U>
    constexpr Vector &operator=(Vector<N, U> const &rhs) noexcept {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = static_cast<T>(rhs[i]);
      return *this;
    }

    constexpr auto const &operator[](size_t i) const noexcept {
      return elements_[i];
    }

    constexpr auto &operator[](size_t i) noexcept {
      return elements_[i];
    }

    constexpr auto size() const noexcept {
      return N;
    }

    constexpr auto data() const noexcept {
      return elements_.data();
    }

    constexpr auto data() noexcept {
      return elements_.data();
    }

    constexpr auto begin() noexcept {
      return elements_.begin();
    }

    constexpr auto begin() const noexcept {
      return elements_.begin();
    }

    constexpr auto cbegin() const noexcept {
      return elements_.cbegin();
    }

    constexpr auto end() noexcept {
      return elements_.end();
    }

    constexpr auto end() const noexcept {
      return elements_.end();
    }

    constexpr auto cend() const noexcept {
      return elements_.cend();
    }

    constexpr auto const &x() const noexcept {
      static_assert(N >= 1);
      return (*this)[0];
    }

    constexpr auto &x() noexcept {
      static_assert(N >= 1);
      return (*this)[0];
    }

    constexpr auto const &y() const noexcept {
      static_assert(N >= 2);
      return (*this)[1];
    }

    constexpr auto &y() noexcept {
      static_assert(N >= 2);
      return (*this)[1];
    }

    constexpr auto const &z() const noexcept {
      static_assert(N >= 3);
      return (*this)[2];
    }

    constexpr auto &z() noexcept {
      static_assert(N >= 3);
      return (*this)[2];
    }

    constexpr auto const &w() const noexcept {
      static_assert(N >= 4);
      return (*this)[3];
    }

    constexpr auto &w() noexcept {
      static_assert(N >= 4);
      return (*this)[3];
    }

  private:
    std::array<T, N> elements_;
  };

  template<typename T>
  using Vector1 = Vector<1, T>;

  template<typename T>
  using Vector2 = Vector<2, T>;

  template<typename T>
  using Vector3 = Vector<3, T>;

  template<typename T>
  using Vector4 = Vector<4, T>;

  using Vector1f = Vector1<float>;
  using Vector2f = Vector2<float>;
  using Vector3f = Vector3<float>;
  using Vector4f = Vector4<float>;
  using Vector1i = Vector1<std::int32_t>;
  using Vector2i = Vector2<std::int32_t>;
  using Vector3i = Vector3<std::int32_t>;
  using Vector4i = Vector4<std::int32_t>;
  using Vector1u = Vector1<std::uint32_t>;
  using Vector2u = Vector2<std::uint32_t>;
  using Vector3u = Vector3<std::uint32_t>;
  using Vector4u = Vector4<std::uint32_t>;

  template<size_t N, typename T>
  constexpr auto makeVector(T x) noexcept {
    auto ret = Vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = x;
    return ret;
  }

  template<typename T>
  constexpr auto makeVector(T x) noexcept {
    auto ret = Vector<1, T>{};
    ret[0] = x;
    return ret;
  }

  template<typename T>
  constexpr auto makeVector(T x, T y) noexcept {
    auto ret = Vector<2, T>{};
    ret[0] = x;
    ret[1] = y;
    return ret;
  }

  template<typename T>
  constexpr auto makeVector(T x, T y, T z) noexcept {
    auto ret = Vector<3, T>{};
    ret[0] = x;
    ret[1] = y;
    ret[2] = z;
    return ret;
  }

  template<typename T>
  constexpr auto makeVector(T x, T y, T z, T w) noexcept {
    auto ret = Vector<4, T>{};
    ret[0] = x;
    ret[1] = y;
    ret[2] = z;
    ret[3] = w;
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto zeroVector() noexcept {
    return Vector<N, T>{};
  }

  constexpr auto zeroVector1f() noexcept {
    return zeroVector<1u, float>();
  }

  constexpr auto zeroVector2f() noexcept {
    return zeroVector<2u, float>();
  }

  constexpr auto zeroVector3f() noexcept {
    return zeroVector<3u, float>();
  }

  constexpr auto zeroVector4f() noexcept {
    return zeroVector<4u, float>();
  }

  constexpr auto zeroVector1i() noexcept {
    return zeroVector<1u, std::int32_t>();
  }

  constexpr auto zeroVector2i() noexcept {
    return zeroVector<2u, std::int32_t>();
  }

  constexpr auto zeroVector3i() noexcept {
    return zeroVector<3u, std::int32_t>();
  }

  constexpr auto zeroVector4i() noexcept {
    return zeroVector<4u, std::int32_t>();
  }

  constexpr auto zeroVector1u() noexcept {
    return zeroVector<1u, std::uint32_t>();
  }

  constexpr auto zeroVector2u() noexcept {
    return zeroVector<2u, std::uint32_t>();
  }

  constexpr auto zeroVector3u() noexcept {
    return zeroVector<3u, std::uint32_t>();
  }

  constexpr auto zeroVector4u() noexcept {
    return zeroVector<4u, std::uint32_t>();
  }

  template<size_t N, typename T, typename U>
  constexpr bool
  operator==(Vector<N, T> const &lhs, Vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      if (lhs[i] != rhs[i])
        return false;
    return true;
  }

  template<size_t N, typename T, typename U>
  constexpr bool
  operator!=(Vector<N, T> const &lhs, Vector<N, U> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<size_t N, typename T>
  constexpr auto operator+(Vector<N, T> const &v) noexcept {
    return v;
  }

  template<size_t N, typename T>
  constexpr auto operator-(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(-v[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = -v[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator*(T lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs * rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator*(Vector<N, T> const &lhs, U rhs) noexcept {
    auto ret = Vector<N, decltype(lhs[0] * rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] * rhs;
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator*=(Vector<N, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] *= rhs;
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  operator*(Vector<N, T> const &lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs[0] * rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] * rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator*=(Vector<N, T> &lhs, Vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] *= rhs[i];
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator/(T lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs / rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs / rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator/(Vector<N, T> const &lhs, U rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
      return lhs * (T{1} / rhs);
    } else if constexpr (std::is_floating_point_v<U>) {
      return lhs * (U{1} / rhs);
    } else {
      auto ret = Vector<N, decltype(lhs[0] / rhs)>{};
      for (auto i = size_t{}; i < N; ++i) {
        ret[i] = lhs[i] / rhs;
      }
      return ret;
    }
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator/=(Vector<N, T> &lhs, U rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>)
      return lhs *= (T{1} / rhs);
    else if constexpr (std::is_floating_point_v<U>)
      return lhs *= (U{1} / rhs);
    else {
      for (auto i = size_t{}; i < N; ++i)
        lhs /= rhs;
      return lhs;
    }
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  operator/(Vector<N, T> const &lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs[0] / rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] / rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator/=(Vector<N, T> &lhs, Vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] /= rhs[i];
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator+(T lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs + rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs + rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator+(Vector<N, T> const &lhs, U rhs) noexcept {
    auto ret = Vector<N, decltype(lhs[0] + rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs;
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator+=(Vector<N, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] += rhs;
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  operator+(Vector<N, T> const &lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs[0] + rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator+=(Vector<N, T> &lhs, Vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] += rhs[i];
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator-(T lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs - rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs - rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator-(Vector<N, T> const &lhs, U rhs) noexcept {
    auto ret = Vector<N, decltype(lhs[0] - rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs;
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator-=(Vector<N, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs;
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  operator-(Vector<N, T> const &lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = Vector<N, decltype(lhs[0] - rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator-=(Vector<N, T> &lhs, Vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs[i];
    return lhs;
  }

  template<size_t N, typename T>
  auto abs(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(abs(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = abs(v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  auto exp(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(exp(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = exp(v[i]);
    return ret;
  }

  template<size_t N, typename T>
  auto log(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(log(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = log(v[i]);
    return ret;
  }

  template<std::size_t N, typename T, typename U>
  auto pow(T base, Vector<N, U> const &exponent) noexcept {
    auto ret = Vector<N, decltype(pow(base, exponent[0]))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = pow(base, exponent[i]);
    }
    return ret;
  }

  template<std::size_t N, typename T, typename U>
  auto pow(Vector<N, T> const &base, U exponent) noexcept {
    auto ret = Vector<N, decltype(pow(base[0], exponent))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = pow(base[i], exponent);
    }
    return ret;
  }

  template<std::size_t N, typename T, typename U>
  auto pow(Vector<N, T> const &base, Vector<N, U> const &exponent) noexcept {
    auto ret = Vector<N, decltype(pow(base[0], exponent[0]))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = pow(base[i], exponent[i]);
    }
    return ret;
  }

  template<std::size_t N, typename T>
  auto sqrt(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(sqrt(v[0]))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = sqrt(v[i]);
    }
    return ret;
  }

  template<std::size_t N, typename T>
  auto cbrt(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(cbrt(v[0]))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = cbrt(v[i]);
    }
    return ret;
  }

  template<std::size_t N, typename T>
  auto sin(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(sin(v[0]))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = sin(v[i]);
    }
    return ret;
  }

  template<std::size_t N, typename T>
  auto cos(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(cos(v[0]))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = cos(v[i]);
    }
    return ret;
  }

  template<std::size_t N, typename T>
  auto tan(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(tan(v[0]))>{};
    for (auto i = std::size_t{}; i < N; ++i) {
      ret[i] = tan(v[i]);
    }
    return ret;
  }

  template<std::size_t N, typename T>
  auto round(Vector<N, T> const &v) noexcept {
    auto ret = Vector<N, decltype(round(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = round(v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto length2(Vector<N, T> const &v) noexcept {
    auto sum = T{};
    for (auto i = size_t{}; i < N; ++i)
      sum += v[i] * v[i];
    return sum;
  }

  template<size_t N, typename T>
  auto length(Vector<N, T> const &v) noexcept {
    return std::sqrt(length2(v));
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  distance2(Vector<N, T> const &p, Vector<N, U> const &q) noexcept {
    return length2(p - q);
  }

  template<size_t N, typename T, typename U>
  auto distance(Vector<N, T> const &p, Vector<N, U> const &q) noexcept {
    return length(p - q);
  }

  template<size_t N, typename T>
  auto normalize(Vector<N, T> const &v) noexcept {
    return v / length(v);
  }

  template<size_t N, typename T, typename U>
  constexpr auto dot(Vector<N, T> const &p, Vector<N, U> const &q) noexcept {
    auto sum = decltype(p[0] * q[0]){};
    for (auto i = size_t{}; i < N; ++i)
      sum += p[i] * q[i];
    return sum;
  }

  template<size_t N, typename T, typename U>
  constexpr auto proj(Vector<N, T> const &p, Vector<N, U> const &q) noexcept {
    return dot(p, q) * q / length2(q);
  }

  template<typename T, typename U, size_t N>
  constexpr auto perp(Vector<N, T> const &p, Vector<N, U> const &q) noexcept {
    return p - proj(p, q);
  }

  template<typename T, typename U>
  constexpr auto cross(Vector3<T> const &p, Vector3<U> const &q) noexcept {
    return makeVector(
        p[1] * q[2] - p[2] * q[1],
        p[2] * q[0] - p[0] * q[2],
        p[0] * q[1] - p[1] * q[0]);
  }

  template<size_t N, typename T>
  constexpr auto min(Vector<N, T> const &v) noexcept {
    auto ret = v[0];
    for (auto i = size_t{1}; i < N; ++i) {
      ret = min(ret, v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(Vector<N, T> const &v) noexcept {
    auto ret = v[0];
    for (auto i = size_t{1}; i < N; ++i) {
      ret = max(ret, v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto min(T a, Vector<N, T> const &b) noexcept {
    auto ret = Vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = min(a, b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto min(Vector<N, T> const &a, T b) noexcept {
    auto ret = Vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = min(a[i], b);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto min(Vector<N, T> const &a, Vector<N, T> const &b) noexcept {
    auto ret = Vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = min(a[i], b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(T a, Vector<N, T> const &b) noexcept {
    auto ret = Vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = max(a, b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(Vector<N, T> const &a, T b) noexcept {
    auto ret = Vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = max(a[i], b);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(Vector<N, T> const &a, Vector<N, T> const &b) noexcept {
    auto ret = Vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = max(a[i], b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto clamp(Vector<N, T> const &x, T a, T b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto
  clamp(Vector<N, T> const &x, T a, Vector<N, T> const &b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto
  clamp(Vector<N, T> const &x, Vector<N, T> const &a, T b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto clamp(
      Vector<N, T> const &x,
      Vector<N, T> const &a,
      Vector<N, T> const &b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto
  lerp(Vector<N, T> const &p, Vector<N, T> const &q, T t) noexcept {
    return (T{1} - t) * p + t * q;
  }

  template<size_t N, typename T>
  auto toUnorm8(Vector<N, T> const &r) noexcept {
    auto i = Vector<N, uint8_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = toUnorm8(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  auto toSnorm8(Vector<N, T> const &r) noexcept {
    auto i = Vector<N, int8_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = toSnorm8(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  auto toUnorm16(Vector<N, T> const &r) noexcept {
    auto i = Vector<N, uint16_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = toUnorm16(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  auto toSnorm16(Vector<N, T> const &r) noexcept {
    auto i = Vector<N, int16_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = toSnorm16(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  constexpr auto toFloat32(Vector<N, T> const &i) noexcept {
    auto r = Vector<N, float>{};
    for (auto n = size_t{}; n < N; ++n) {
      r[n] = toFloat32(i[n]);
    }
    return r;
  }

  template<size_t N, typename T>
  constexpr auto toFloat64(Vector<N, T> const &i) noexcept {
    auto r = Vector<N, double>{};
    for (auto n = size_t{}; n < N; ++n) {
      r[n] = toFloat64(i[n]);
    }
    return r;
  }

  template<typename T>
  auto encodeOctahedral(Vector<3, T> const &v) noexcept {
    auto p = makeVector(v[0], v[1]) / (abs(v[0]) + abs(v[1]) + abs(v[2]));
    return v[2] <= T{0} ? (T{1} - makeVector(abs(p[1]), abs(p[0]))) *
                              makeVector(
                                  p[0] >= T{0} ? T{1} : T{-1},
                                  p[1] >= T{0} ? T{1} : T{-1})
                        : p;
  }

  template<typename T>
  auto decodeOctahedral(Vector<2, T> const &e) noexcept {
    auto v = makeVector(e[0], e[1], T{1} - abs(e[0]) - abs(e[1]));
    if (v[2] < T{0}) {
      v[0] = (T{1} - abs(e[1])) * (e[0] >= T{0} ? T{1} : T{-1});
      v[1] = (T{1} - abs(e[0])) * (e[1] >= T{0} ? T{1} : T{-1});
    }
    return normalize(v);
  }

  template<size_t NewSize, size_t OldSize, typename T>
  constexpr auto resize(Vector<OldSize, T> const &v, T x = T{}) noexcept {
    auto ret = Vector<NewSize, T>{};
    if constexpr (NewSize > OldSize) {
      for (auto i = size_t{}; i < OldSize; ++i) {
        ret[i] = v[i];
      }
      for (auto i = OldSize; i < NewSize; ++i) {
        ret[i] = x;
      }
    } else {
      for (auto i = size_t{}; i < NewSize; ++i) {
        ret[i] = v[i];
      }
    }
    return ret;
  }

  template<size_t First, size_t Last, size_t N, typename T>
  constexpr auto slice(Vector<N, T> const &v) noexcept {
    static_assert(First < Last && Last <= N);
    auto ret = Vector<Last - First, T>{};
    for (auto i = First; i < Last; ++i) {
      ret[i - First] = v[i];
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto concatenate(T x, Vector<N, T> const &v) noexcept {
    auto ret = Vector<N + 1, T>{};
    ret[0] = x;
    for (auto i = size_t{}; i < N; ++i) {
      ret[i + 1] = v[i];
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto concatenate(Vector<N, T> const &v, T x) noexcept {
    auto ret = Vector<N + 1, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = v[i];
    }
    ret[N] = x;
    return ret;
  }

  template<size_t N, size_t M, typename T>
  constexpr auto
  concatenate(Vector<N, T> const &p, Vector<M, T> const &q) noexcept {
    auto ret = Vector<N + M, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = p[i];
    }
    for (auto i = size_t{}; i < M; ++i) {
      ret[i + N] = q[i];
    }
    return ret;
  }

  template<size_t N, typename T>
  auto &operator<<(std::ostream &os, Vector<N, T> const &v) noexcept {
    os << "[" << v[0];
    for (auto i = size_t{1}; i < N; ++i) {
      os << ", " << v[i];
    }
    return os << "]";
  }

  template<size_t N, typename T, typename H>
  H AbslHashValue(H state, Vector<N, T> const &v) noexcept {
    return H::combine_contiguous(std::move(state), v.data(), v.size());
  }

} // namespace imp
