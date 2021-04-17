#pragma once

#include <cmath>

#include <array>
#include <ostream>
#include <type_traits>

#include <boost/container_hash/hash.hpp>

#include "scalar.h"

namespace imp {
  /**
   * @brief An N-element vector.
   */
  template<size_t N, typename T>
  class vector {
  public:
    using scalar_t = T;

    constexpr vector() noexcept: elements_{} {}

    constexpr vector(T x) noexcept: elements_{} {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = x;
    }

    constexpr vector &operator=(T x) noexcept {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = x;
    }

    template<typename U>
    constexpr vector(vector<N, U> const &rhs) noexcept: elements_{} {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = static_cast<T>(rhs[i]);
    }

    template<typename U>
    constexpr vector &operator=(vector<N, U> const &rhs) noexcept {
      for (auto i = size_t{}; i < N; ++i)
        elements_[i] = static_cast<T>(rhs[i]);
      return *this;
    }

    constexpr operator bool() const noexcept {
      for (auto i = size_t{}; i < N; ++i) {
        if (elements_[i]) {
          return true;
        }
      }
      return false;
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
  using vector1 = vector<1, T>;
  template<typename T>
  using vector2 = vector<2, T>;
  template<typename T>
  using vector3 = vector<3, T>;
  template<typename T>
  using vector4 = vector<4, T>;
  using vector1f = vector1<float>;
  using vector2f = vector2<float>;
  using vector3f = vector3<float>;
  using vector4f = vector4<float>;
  using vector1i = vector1<int32_t>;
  using vector2i = vector2<int32_t>;
  using vector3i = vector3<int32_t>;
  using vector4i = vector4<int32_t>;
  using vector1u = vector1<uint32_t>;
  using vector2u = vector2<uint32_t>;
  using vector3u = vector3<uint32_t>;
  using vector4u = vector4<uint32_t>;

  template<size_t N, typename T>
  constexpr auto make_vector(T x) noexcept {
    auto ret = vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = x;
    return ret;
  }

  template<typename T>
  constexpr auto make_vector(T x) noexcept {
    auto ret = vector<1, T>{};
    ret[0] = x;
    return ret;
  }

  template<typename T>
  constexpr auto make_vector(T x, T y) noexcept {
    auto ret = vector<2, T>{};
    ret[0] = x;
    ret[1] = y;
    return ret;
  }

  template<typename T>
  constexpr auto make_vector(T x, T y, T z) noexcept {
    auto ret = vector<3, T>{};
    ret[0] = x;
    ret[1] = y;
    ret[2] = z;
    return ret;
  }

  template<typename T>
  constexpr auto make_vector(T x, T y, T z, T w) noexcept {
    auto ret = vector<4, T>{};
    ret[0] = x;
    ret[1] = y;
    ret[2] = z;
    ret[3] = w;
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto zero_vector() noexcept {
    return vector<N, T>{};
  }

  template<size_t N, typename T, typename U>
  constexpr bool
  operator==(vector<N, T> const &lhs, vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      if (lhs[i] != rhs[i])
        return false;
    return true;
  }

  template<size_t N, typename T, typename U>
  constexpr bool
  operator!=(vector<N, T> const &lhs, vector<N, U> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<size_t N, typename T>
  constexpr auto operator+(vector<N, T> const &v) noexcept {
    return v;
  }

  template<size_t N, typename T>
  constexpr auto operator-(vector<N, T> const &v) noexcept {
    auto ret = vector<N, decltype(-v[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = -v[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator*(T lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs * rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator*(vector<N, T> const &lhs, U rhs) noexcept {
    auto ret = vector<N, decltype(lhs[0] * rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] * rhs;
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator*=(vector<N, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] *= rhs;
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  operator*(vector<N, T> const &lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs[0] * rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] * rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator*=(vector<N, T> &lhs, vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] *= rhs[i];
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator/(T lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs / rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs / rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator/(vector<N, T> const &lhs, U rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
      return lhs * (T{1} / rhs);
    } else if constexpr (std::is_floating_point_v<U>) {
      return lhs * (U{1} / rhs);
    } else {
      auto ret = vector<N, decltype(lhs[0] / rhs)>{};
      for (auto i = size_t{}; i < N; ++i) {
        ret[i] = lhs[i] / rhs;
      }
      return ret;
    }
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator/=(vector<N, T> &lhs, U rhs) noexcept {
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
  operator/(vector<N, T> const &lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs[0] / rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] / rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator/=(vector<N, T> &lhs, vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] /= rhs[i];
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator+(T lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs + rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs + rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator+(vector<N, T> const &lhs, U rhs) noexcept {
    auto ret = vector<N, decltype(lhs[0] + rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs;
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator+=(vector<N, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] += rhs;
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  operator+(vector<N, T> const &lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs[0] + rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator+=(vector<N, T> &lhs, vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] += rhs[i];
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator-(T lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs - rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs - rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto operator-(vector<N, T> const &lhs, U rhs) noexcept {
    auto ret = vector<N, decltype(lhs[0] - rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs;
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &operator-=(vector<N, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs;
    return lhs;
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  operator-(vector<N, T> const &lhs, vector<N, U> const &rhs) noexcept {
    auto ret = vector<N, decltype(lhs[0] - rhs[0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs[i];
    return ret;
  }

  template<size_t N, typename T, typename U>
  constexpr auto &
  operator-=(vector<N, T> &lhs, vector<N, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs[i];
    return lhs;
  }

  template<size_t N, typename T>
  auto exp(vector<N, T> const &v) noexcept {
    auto ret = vector<N, decltype(std::exp(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = std::exp(v[i]);
    return ret;
  }

  template<size_t N, typename T>
  auto log(vector<N, T> const &v) noexcept {
    auto ret = vector<N, decltype(std::log(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = std::log(v[i]);
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto length2(vector<N, T> const &v) noexcept {
    auto sum = T{};
    for (auto i = size_t{}; i < N; ++i)
      sum += v[i] * v[i];
    return sum;
  }

  template<size_t N, typename T>
  auto length(vector<N, T> const &v) noexcept {
    return std::sqrt(length2(v));
  }

  template<size_t N, typename T, typename U>
  constexpr auto
  distance2(vector<N, T> const &p, vector<N, U> const &q) noexcept {
    return length2(p - q);
  }

  template<size_t N, typename T, typename U>
  auto distance(vector<N, T> const &p, vector<N, U> const &q) noexcept {
    return length(p - q);
  }

  template<size_t N, typename T>
  auto normalize(vector<N, T> const &v) noexcept {
    return v / length(v);
  }

  template<size_t N, typename T, typename U>
  constexpr auto dot(vector<N, T> const &p, vector<N, U> const &q) noexcept {
    auto sum = decltype(p[0] * q[0]){};
    for (auto i = size_t{}; i < N; ++i)
      sum += p[i] * q[i];
    return sum;
  }

  template<size_t N, typename T, typename U>
  constexpr auto proj(vector<N, T> const &p, vector<N, U> const &q) noexcept {
    return dot(p, q) * q / length2(q);
  }

  template<typename T, typename U, size_t N>
  constexpr auto perp(vector<N, T> const &p, vector<N, U> const &q) noexcept {
    return p - proj(p, q);
  }

  template<typename T, typename U>
  constexpr auto cross(vector3<T> const &p, vector3<U> const &q) noexcept {
    return make_vector(
        p[1] * q[2] - p[2] * q[1],
        p[2] * q[0] - p[0] * q[2],
        p[0] * q[1] - p[1] * q[0]);
  }

  template<size_t N, typename T>
  auto abs(vector<N, T> const &v) noexcept {
    auto ret = vector<N, decltype(abs(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = abs(v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  auto round(vector<N, T> const &v) noexcept {
    auto ret = vector<N, decltype(round(v[0]))>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = round(v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto min(vector<N, T> const &v) noexcept {
    auto ret = v[0];
    for (auto i = size_t{1}; i < N; ++i) {
      ret = min(ret, v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(vector<N, T> const &v) noexcept {
    auto ret = v[0];
    for (auto i = size_t{1}; i < N; ++i) {
      ret = max(ret, v[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto min(T a, vector<N, T> const &b) noexcept {
    auto ret = vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = min(a, b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto min(vector<N, T> const &a, T b) noexcept {
    auto ret = vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = min(a[i], b);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto min(vector<N, T> const &a, vector<N, T> const &b) noexcept {
    auto ret = vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = min(a[i], b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(T a, vector<N, T> const &b) noexcept {
    auto ret = vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = max(a, b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(vector<N, T> const &a, T b) noexcept {
    auto ret = vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = max(a[i], b);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto max(vector<N, T> const &a, vector<N, T> const &b) noexcept {
    auto ret = vector<N, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = max(a[i], b[i]);
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto clamp(vector<N, T> const &x, T a, T b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto
  clamp(vector<N, T> const &x, T a, vector<N, T> const &b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto
  clamp(vector<N, T> const &x, vector<N, T> const &a, T b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto clamp(
      vector<N, T> const &x,
      vector<N, T> const &a,
      vector<N, T> const &b) noexcept {
    return min(max(x, a), b);
  }

  template<size_t N, typename T>
  constexpr auto
  lerp(vector<N, T> const &p, vector<N, T> const &q, T t) noexcept {
    return (T{1} - t) * p + t * q;
  }

  template<size_t N, typename T>
  auto to_unorm8(vector<N, T> const &r) noexcept {
    auto i = vector<N, uint8_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = to_unorm8(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  auto to_snorm8(vector<N, T> const &r) noexcept {
    auto i = vector<N, int8_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = to_snorm8(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  auto to_unorm16(vector<N, T> const &r) noexcept {
    auto i = vector<N, uint16_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = to_unorm16(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  auto to_snorm16(vector<N, T> const &r) noexcept {
    auto i = vector<N, int16_t>{};
    for (auto n = size_t{}; n < N; ++n) {
      i[n] = to_snorm16(r[n]);
    }
    return i;
  }

  template<size_t N, typename T>
  constexpr auto to_f32(vector<N, T> const &i) noexcept {
    auto r = vector<N, float>{};
    for (auto n = size_t{}; n < N; ++n) {
      r[n] = to_f32(i[n]);
    }
    return r;
  }

  template<size_t N, typename T>
  constexpr auto to_f64(vector<N, T> const &i) noexcept {
    auto r = vector<N, double>{};
    for (auto n = size_t{}; n < N; ++n) {
      r[n] = to_f64(i[n]);
    }
    return r;
  }

  template<typename T>
  auto encode_octahedral(vector<3, T> const &v) noexcept {
    auto p = make_vector(v[0], v[1]) / (abs(v[0]) + abs(v[1]) + abs(v[2]));
    return v[2] <= T{0} ? (T{1} - make_vector(abs(p[1]), abs(p[0]))) *
                              make_vector(
                                  p[0] >= T{0} ? T{1} : T{-1},
                                  p[1] >= T{0} ? T{1} : T{-1})
                        : p;
  }

  template<typename T>
  auto decode_octahedral(vector<2, T> const &e) noexcept {
    auto v = make_vector(e[0], e[1], T{1} - abs(e[0]) - abs(e[1]));
    if (v[2] < T{0}) {
      v[0] = (T{1} - abs(e[1])) * (e[0] >= T{0} ? T{1} : T{-1});
      v[1] = (T{1} - abs(e[0])) * (e[1] >= T{0} ? T{1} : T{-1});
    }
    return normalize(v);
  }

  template<size_t NewSize, size_t OldSize, typename T>
  constexpr auto resize(vector<OldSize, T> const &v, T x = T{}) noexcept {
    auto ret = vector<NewSize, T>{};
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
  constexpr auto slice(vector<N, T> const &v) noexcept {
    static_assert(First < Last && Last <= N);
    auto ret = vector<Last - First, T>{};
    for (auto i = First; i < Last; ++i) {
      ret[i - First] = v[i];
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto concatenate(T x, vector<N, T> const &v) noexcept {
    auto ret = vector<N + 1, T>{};
    ret[0] = x;
    for (auto i = size_t{}; i < N; ++i) {
      ret[i + 1] = v[i];
    }
    return ret;
  }

  template<size_t N, typename T>
  constexpr auto concatenate(vector<N, T> const &v, T x) noexcept {
    auto ret = vector<N + 1, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = v[i];
    }
    ret[N] = x;
    return ret;
  }

  template<size_t N, size_t M, typename T>
  constexpr auto
  concatenate(vector<N, T> const &p, vector<M, T> const &q) noexcept {
    auto ret = vector<N + M, T>{};
    for (auto i = size_t{}; i < N; ++i) {
      ret[i] = p[i];
    }
    for (auto i = size_t{}; i < M; ++i) {
      ret[i + N] = q[i];
    }
    return ret;
  }

  template<size_t N, typename T>
  auto &operator<<(std::ostream &os, vector<N, T> const &v) noexcept {
    os << "[" << v[0];
    for (auto i = size_t{1}; i < N; ++i) {
      os << ", " << v[i];
    }
    return os << "]";
  }

  template<size_t N, typename T>
  size_t hash_value(vector<N, T> const &v) noexcept {
    return boost::hash_range(v.cbegin(), v.cend());
  }
} // namespace imp

namespace std {
  template<size_t N, typename T>
  struct hash<imp::vector<N, T>> {
    size_t operator()(imp::vector<N, T> const &v) const noexcept {
      return hash_value(v);
    }
  };
} // namespace std
