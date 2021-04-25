#pragma once

#include <limits>

#include "Matrix.h"

namespace imp {
  /**
   * @brief An N-dimensional Interval.
   */
  template<size_t N, typename T>
  class Interval {
  public:
    Vector<N, T> min;
    Vector<N, T> max;

    /**
     * @brief Constructs an empty Interval.
     */
    constexpr Interval() noexcept:
        min{std::numeric_limits<T>::max()},
        max{std::numeric_limits<T>::lowest()} {}

    constexpr Interval(Vector<N, T> p) noexcept: min{p}, max{p} {}

    constexpr Interval(Vector<N, T> min, Vector<N, T> max) noexcept:
        min{min}, max{max} {}

    /**
     * @returns true if the Interval is non-empty, false otherwise.
     */
    constexpr operator bool() const noexcept {
      for (auto i = size_t{}; i < N; ++i)
        if (max[i] < min[i])
          return false;
      return true;
    }
  };

  template<typename T>
  using Interval1 = Interval<1, T>;

  template<typename T>
  using Interval2 = Interval<2, T>;

  template<typename T>
  using Interval3 = Interval<3, T>;

  using Interval1f = Interval1<float>;
  using Interval2f = Interval2<float>;
  using Interval3f = Interval3<float>;
  using Interval1i = Interval1<std::int32_t>;
  using Interval2i = Interval2<std::int32_t>;
  using Interval3i = Interval3<std::int32_t>;
  using Interval1u = Interval1<std::uint32_t>;
  using Interval2u = Interval2<std::uint32_t>;
  using Interval3u = Interval3<std::uint32_t>;

  template<typename T, size_t N>
  constexpr bool
  operator==(Interval<N, T> const &lhs, Interval<N, T> const &rhs) noexcept {
    return lhs.min == rhs.min && lhs.max == rhs.max;
  }

  template<typename T, size_t N>
  constexpr bool
  operator!=(Interval<N, T> const &lhs, Interval<N, T> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<typename T, size_t N>
  constexpr auto size(Interval<N, T> const &i) noexcept {
    auto ret = T{1};
    for (auto n = size_t{}; n < N; ++n)
      ret *= i.max[n] - i.min[n];
    return ret;
  }

  template<typename T, size_t N>
  constexpr auto expand(Interval<N, T> const &i, T x) noexcept {
    return Interval{i.min - x, i.max + x};
  }

  template<typename T, size_t N>
  constexpr auto shrink(Interval<N, T> const &i, T x) noexcept {
    return Interval{i.min + x, i.max - x};
  }

  template<typename T, size_t N>
  constexpr auto
  operator&(Interval<N, T> const &lhs, Interval<N, T> const &rhs) noexcept {
    return Interval<N, T>{max(lhs.min, rhs.min), min(lhs.max, rhs.max)};
  }

  template<typename T, size_t N>
  constexpr auto &
  operator&=(Interval<N, T> &lhs, Interval<N, T> const &rhs) noexcept {
    return lhs = lhs & rhs;
  }

  template<typename T, size_t N>
  constexpr auto
  operator|(Vector<N, T> const &lhs, Interval<N, T> const &rhs) noexcept {
    return Interval<N, T>{min(lhs, rhs.min), max(lhs, rhs.max)};
  }

  template<typename T, size_t N>
  constexpr auto
  operator|(Interval<N, T> const &lhs, Vector<N, T> const &rhs) noexcept {
    return rhs | lhs;
  }

  template<typename T, size_t N>
  constexpr auto &
  operator|=(Interval<N, T> &lhs, Vector<N, T> const &rhs) noexcept {
    return lhs = lhs | rhs;
  }

  template<typename T, size_t N>
  constexpr auto
  operator|(Interval<N, T> const &lhs, Interval<N, T> const &rhs) noexcept {
    return Interval<N, T>{min(lhs.min, rhs.min), max(lhs.max, rhs.max)};
  }

  template<typename T, size_t N>
  constexpr auto &
  operator|=(Interval<N, T> &lhs, Interval<N, T> const &rhs) noexcept {
    return lhs = lhs | rhs;
  }

  template<typename T, size_t N>
  constexpr bool
  contains(Interval<N, T> const &i, Vector<N, T> const &p) noexcept {
    for (auto n = size_t{}; n < N; ++n)
      if (i.max[n] < p[n] || i.min[n] > p[n])
        return false;
    return true;
  }

  template<typename T, size_t N>
  constexpr bool
  contains(Interval<N, T> const &i1, Interval<N, T> const &i2) noexcept {
    for (auto n = size_t{}; n < N; ++n)
      if (i1.max[n] < i2.max[n] || i1.min[n] > i2.min[n])
        return false;
    return true;
  }

  template<typename T, size_t N>
  constexpr bool
  overlaps(Interval<N, T> const &i1, Interval<N, T> const &i2) noexcept {
    for (auto n = size_t{}; n < N; ++n)
      if (i1.max[n] < i2.min[n] || i1.min[n] > i2.max[n])
        return false;
    return true;
  }

  template<typename T>
  constexpr auto
  transform(Interval3<T> const &i, Matrix3x3<T> const &m) noexcept {
    auto ret = Interval3<T>{};
    ret |= m * makeVector(i.min[0], i.min[1], i.min[2]);
    ret |= m * makeVector(i.min[0], i.min[1], i.max[2]);
    ret |= m * makeVector(i.min[0], i.max[1], i.min[2]);
    ret |= m * makeVector(i.min[0], i.max[1], i.max[2]);
    ret |= m * makeVector(i.max[0], i.min[1], i.min[2]);
    ret |= m * makeVector(i.max[0], i.min[1], i.max[2]);
    ret |= m * makeVector(i.max[0], i.max[1], i.min[2]);
    ret |= m * makeVector(i.max[0], i.max[1], i.max[2]);
    return ret;
  }

  template<typename T>
  constexpr auto
  transform(Interval3<T> const &i, Matrix4x3<T> const &m) noexcept {
    auto ret = Interval3<T>{};
    ret |= m * makeVector(i.min[0], i.min[1], i.min[2], T{1});
    ret |= m * makeVector(i.min[0], i.min[1], i.max[2], T{1});
    ret |= m * makeVector(i.min[0], i.max[1], i.min[2], T{1});
    ret |= m * makeVector(i.min[0], i.max[1], i.max[2], T{1});
    ret |= m * makeVector(i.max[0], i.min[1], i.min[2], T{1});
    ret |= m * makeVector(i.max[0], i.min[1], i.max[2], T{1});
    ret |= m * makeVector(i.max[0], i.max[1], i.min[2], T{1});
    ret |= m * makeVector(i.max[0], i.max[1], i.max[2], T{1});
    return ret;
  }

  template<typename T>
  constexpr auto
  transform(Interval3<T> const &i, Matrix4x4<T> const &m) noexcept {
    auto ret = Interval3<T>{};
    ret |= resize<3>(m * makeVector(i.min[0], i.min[1], i.min[2], T{1}));
    ret |= resize<3>(m * makeVector(i.min[0], i.min[1], i.max[2], T{1}));
    ret |= resize<3>(m * makeVector(i.min[0], i.max[1], i.min[2], T{1}));
    ret |= resize<3>(m * makeVector(i.min[0], i.max[1], i.max[2], T{1}));
    ret |= resize<3>(m * makeVector(i.max[0], i.min[1], i.min[2], T{1}));
    ret |= resize<3>(m * makeVector(i.max[0], i.min[1], i.max[2], T{1}));
    ret |= resize<3>(m * makeVector(i.max[0], i.max[1], i.min[2], T{1}));
    ret |= resize<3>(m * makeVector(i.max[0], i.max[1], i.max[2], T{1}));
    return ret;
  }

  template<typename T, size_t N>
  auto &operator<<(std::ostream &os, Interval<N, T> const &i) {
    return os << "[" << i.min << ", " << i.max << "]";
  }
} // namespace imp
