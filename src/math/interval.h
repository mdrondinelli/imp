#pragma once

#include <limits>

#include "matrix.h"

namespace imp {
  /**
   * @brief An N-dimensional interval.
   */
  template<size_t N, typename T>
  class interval {
  public:
    vector<N, T> min;
    vector<N, T> max;

    /**
     * @brief Constructs an empty interval.
     */
    constexpr interval() noexcept:
        min{std::numeric_limits<T>::max()},
        max{std::numeric_limits<T>::lowest()} {}

    constexpr interval(vector<N, T> p) noexcept: min{p}, max{p} {}

    constexpr interval(vector<N, T> min, vector<N, T> max) noexcept:
        min{min}, max{max} {}

    /**
     * @returns true if the interval is non-empty, false otherwise.
     */
    constexpr operator bool() const noexcept {
      for (auto i = size_t{}; i < N; ++i)
        if (max[i] < min[i])
          return false;
      return true;
    }
  };

  template<typename T>
  using interval1 = interval<1, T>;

  template<typename T>
  using interval2 = interval<2, T>;

  template<typename T>
  using interval3 = interval<3, T>;

  template<typename T, size_t N>
  constexpr bool
  operator==(interval<N, T> const &lhs, interval<N, T> const &rhs) noexcept {
    return lhs.min == rhs.min && lhs.max == rhs.max;
  }

  template<typename T, size_t N>
  constexpr bool
  operator!=(interval<N, T> const &lhs, interval<N, T> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<typename T, size_t N>
  constexpr auto size(interval<N, T> const &i) noexcept {
    auto ret = T{1};
    for (auto n = size_t{}; n < N; ++n)
      ret *= i.max[n] - i.min[n];
    return ret;
  }

  template<typename T, size_t N>
  constexpr auto expand(interval<N, T> const &i, T x) noexcept {
    return interval{i.min - x, i.max + x};
  }

  template<typename T, size_t N>
  constexpr auto shrink(interval<N, T> const &i, T x) noexcept {
    return interval{i.min + x, i.max - x};
  }

  template<typename T, size_t N>
  constexpr auto
  operator&(interval<N, T> const &lhs, interval<N, T> const &rhs) noexcept {
    return interval<N, T>{max(lhs.min, rhs.min), min(lhs.max, rhs.max)};
  }

  template<typename T, size_t N>
  constexpr auto &
  operator&=(interval<N, T> &lhs, interval<N, T> const &rhs) noexcept {
    return lhs = lhs & rhs;
  }

  template<typename T, size_t N>
  constexpr auto
  operator|(vector<N, T> const &lhs, interval<N, T> const &rhs) noexcept {
    return interval<N, T>{min(lhs, rhs.min), max(lhs, rhs.max)};
  }

  template<typename T, size_t N>
  constexpr auto
  operator|(interval<N, T> const &lhs, vector<N, T> const &rhs) noexcept {
    return rhs | lhs;
  }

  template<typename T, size_t N>
  constexpr auto &
  operator|=(interval<N, T> &lhs, vector<N, T> const &rhs) noexcept {
    return lhs = lhs | rhs;
  }

  template<typename T, size_t N>
  constexpr auto
  operator|(interval<N, T> const &lhs, interval<N, T> const &rhs) noexcept {
    return interval<N, T>{min(lhs.min, rhs.min), max(lhs.max, rhs.max)};
  }

  template<typename T, size_t N>
  constexpr auto &
  operator|=(interval<N, T> &lhs, interval<N, T> const &rhs) noexcept {
    return lhs = lhs | rhs;
  }

  template<typename T, size_t N>
  constexpr bool
  contains(interval<N, T> const &i, vector<N, T> const &p) noexcept {
    for (auto n = size_t{}; n < N; ++n)
      if (i.max[n] < p[n] || i.min[n] > p[n])
        return false;
    return true;
  }

  template<typename T, size_t N>
  constexpr bool
  contains(interval<N, T> const &i1, interval<N, T> const &i2) noexcept {
    for (auto n = size_t{}; n < N; ++n)
      if (i1.max[n] < i2.max[n] || i1.min[n] > i2.min[n])
        return false;
    return true;
  }

  template<typename T, size_t N>
  constexpr bool
  overlaps(interval<N, T> const &i1, interval<N, T> const &i2) noexcept {
    for (auto n = size_t{}; n < N; ++n)
      if (i1.max[n] < i2.min[n] || i1.min[n] > i2.max[n])
        return false;
    return true;
  }

  template<typename T>
  constexpr auto
  transform(interval3<T> const &i, matrix3x3<T> const &m) noexcept {
    auto ret = interval3<T>{};
    ret |= m * make_vector(i.min[0], i.min[1], i.min[2]);
    ret |= m * make_vector(i.min[0], i.min[1], i.max[2]);
    ret |= m * make_vector(i.min[0], i.max[1], i.min[2]);
    ret |= m * make_vector(i.min[0], i.max[1], i.max[2]);
    ret |= m * make_vector(i.max[0], i.min[1], i.min[2]);
    ret |= m * make_vector(i.max[0], i.min[1], i.max[2]);
    ret |= m * make_vector(i.max[0], i.max[1], i.min[2]);
    ret |= m * make_vector(i.max[0], i.max[1], i.max[2]);
    return ret;
  }

  template<typename T>
  constexpr auto
  transform(interval3<T> const &i, matrix4x3<T> const &m) noexcept {
    auto ret = interval3<T>{};
    ret |= m * make_vector(i.min[0], i.min[1], i.min[2], T{1});
    ret |= m * make_vector(i.min[0], i.min[1], i.max[2], T{1});
    ret |= m * make_vector(i.min[0], i.max[1], i.min[2], T{1});
    ret |= m * make_vector(i.min[0], i.max[1], i.max[2], T{1});
    ret |= m * make_vector(i.max[0], i.min[1], i.min[2], T{1});
    ret |= m * make_vector(i.max[0], i.min[1], i.max[2], T{1});
    ret |= m * make_vector(i.max[0], i.max[1], i.min[2], T{1});
    ret |= m * make_vector(i.max[0], i.max[1], i.max[2], T{1});
    return ret;
  }

  template<typename T>
  constexpr auto
  transform(interval3<T> const &i, matrix4x4<T> const &m) noexcept {
    auto ret = interval3<T>{};
    ret |= resize<3>(m * make_vector(i.min[0], i.min[1], i.min[2], T{1}));
    ret |= resize<3>(m * make_vector(i.min[0], i.min[1], i.max[2], T{1}));
    ret |= resize<3>(m * make_vector(i.min[0], i.max[1], i.min[2], T{1}));
    ret |= resize<3>(m * make_vector(i.min[0], i.max[1], i.max[2], T{1}));
    ret |= resize<3>(m * make_vector(i.max[0], i.min[1], i.min[2], T{1}));
    ret |= resize<3>(m * make_vector(i.max[0], i.min[1], i.max[2], T{1}));
    ret |= resize<3>(m * make_vector(i.max[0], i.max[1], i.min[2], T{1}));
    ret |= resize<3>(m * make_vector(i.max[0], i.max[1], i.max[2], T{1}));
    return ret;
  }

  template<typename T, size_t N>
  auto &operator<<(std::ostream &os, interval<N, T> const &i) {
    return os << "[" << i.min << ", " << i.max << "]";
  }

  using interval1i = interval1<int>;
  using interval2i = interval2<int>;
  using interval3i = interval3<int>;
  using interval1f = interval1<float>;
  using interval2f = interval2<float>;
  using interval3f = interval3<float>;
} // namespace imp
