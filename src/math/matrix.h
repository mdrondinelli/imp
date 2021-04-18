#pragma once

#include <stdexcept>
#include <utility>

#include "quaternion.h"
#include "vector.h"

namespace imp {
  /**
   * @brief an NxM (n columns, m rows) matrix.
   */
  template<size_t N, size_t M, typename T>
  class matrix {
  public:
    using column_t = vector<M, T>;
    using scalar_t = T;

    constexpr matrix() noexcept: cols_{} {}

    template<typename U>
    constexpr matrix(matrix<N, M, U> const &rhs) noexcept: cols_{} {
      for (auto i = size_t{}; i < N; ++i)
        cols_[i] = rhs.cols_[i];
    }

    template<typename U>
    constexpr matrix &operator=(matrix<N, M, U> const &rhs) noexcept {
      for (auto i = size_t{}; i < N; ++i)
        cols_[i] = rhs.cols_[i];
    }

    constexpr size_t cols() const noexcept {
      return N;
    }

    constexpr size_t rows() const noexcept {
      return M;
    }

    constexpr auto begin() noexcept {
      return cols_.begin();
    }

    constexpr auto begin() const noexcept {
      return cols_.begin();
    }

    constexpr auto cbegin() const noexcept {
      return cols_.cbegin();
    }

    constexpr auto end() noexcept {
      return cols_.end();
    }

    constexpr auto end() const noexcept {
      return cols_.end();
    }

    constexpr auto cend() const noexcept {
      return cols_.cend();
    }

    constexpr auto const &operator[](size_t i) const noexcept {
      return cols_[i];
    }

    constexpr auto &operator[](size_t i) noexcept {
      return cols_[i];
    }

  private:
    std::array<column_t, N> cols_;
  };

  template<typename T>
  using matrix1x1 = matrix<1, 1, T>;

  template<typename T>
  using matrix1x2 = matrix<1, 2, T>;

  template<typename T>
  using matrix1x3 = matrix<1, 3, T>;

  template<typename T>
  using matrix1x4 = matrix<1, 4, T>;

  template<typename T>
  using matrix2x1 = matrix<2, 1, T>;

  template<typename T>
  using matrix2x2 = matrix<2, 2, T>;

  template<typename T>
  using matrix2x3 = matrix<2, 3, T>;

  template<typename T>
  using matrix2x4 = matrix<2, 4, T>;

  template<typename T>
  using matrix3x1 = matrix<3, 1, T>;

  template<typename T>
  using matrix3x2 = matrix<3, 2, T>;

  template<typename T>
  using matrix3x3 = matrix<3, 3, T>;

  template<typename T>
  using matrix3x4 = matrix<3, 4, T>;

  template<typename T>
  using matrix4x1 = matrix<4, 1, T>;

  template<typename T>
  using matrix4x2 = matrix<4, 2, T>;

  template<typename T>
  using matrix4x3 = matrix<4, 3, T>;

  template<typename T>
  using matrix4x4 = matrix<4, 4, T>;

  using matrix1x1f = matrix1x1<float>;
  using matrix1x2f = matrix1x2<float>;
  using matrix1x3f = matrix1x2<float>;
  using matrix1x4f = matrix1x4<float>;
  using matrix2x1f = matrix2x1<float>;
  using matrix2x2f = matrix2x2<float>;
  using matrix2x3f = matrix2x3<float>;
  using matrix2x4f = matrix2x4<float>;
  using matrix3x1f = matrix3x1<float>;
  using matrix3x2f = matrix3x2<float>;
  using matrix3x3f = matrix3x3<float>;
  using matrix4x1f = matrix4x1<float>;
  using matrix4x2f = matrix4x2<float>;
  using matrix4x3f = matrix4x3<float>;
  using matrix4x4f = matrix4x4<float>;
  using matrix1x1i = matrix1x1<std::int32_t>;
  using matrix1x2i = matrix1x2<std::int32_t>;
  using matrix1x3i = matrix1x2<std::int32_t>;
  using matrix1x4i = matrix1x4<std::int32_t>;
  using matrix2x1i = matrix2x1<std::int32_t>;
  using matrix2x2i = matrix2x2<std::int32_t>;
  using matrix2x3i = matrix2x3<std::int32_t>;
  using matrix2x4i = matrix2x4<std::int32_t>;
  using matrix3x1i = matrix3x1<std::int32_t>;
  using matrix3x2i = matrix3x2<std::int32_t>;
  using matrix3x3i = matrix3x3<std::int32_t>;
  using matrix4x1i = matrix4x1<std::int32_t>;
  using matrix4x2i = matrix4x2<std::int32_t>;
  using matrix4x3i = matrix4x3<std::int32_t>;
  using matrix4x4i = matrix4x4<std::int32_t>;
  using matrix1x1u = matrix1x1<std::uint32_t>;
  using matrix1x2u = matrix1x2<std::uint32_t>;
  using matrix1x3u = matrix1x2<std::uint32_t>;
  using matrix1x4u = matrix1x4<std::uint32_t>;
  using matrix2x1u = matrix2x1<std::uint32_t>;
  using matrix2x2u = matrix2x2<std::uint32_t>;
  using matrix2x3u = matrix2x3<std::uint32_t>;
  using matrix2x4u = matrix2x4<std::uint32_t>;
  using matrix3x1u = matrix3x1<std::uint32_t>;
  using matrix3x2u = matrix3x2<std::uint32_t>;
  using matrix3x3u = matrix3x3<std::uint32_t>;
  using matrix4x1u = matrix4x1<std::uint32_t>;
  using matrix4x2u = matrix4x2<std::uint32_t>;
  using matrix4x3u = matrix4x3<std::uint32_t>;
  using matrix4x4u = matrix4x4<std::uint32_t>;

  template<size_t N = 1, size_t M, typename T>
  constexpr auto make_matrix(vector<M, T> const &c) noexcept {
    auto ret = matrix<N, M, T>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = c;
    return ret;
  }

  template<size_t M, typename T>
  constexpr auto
  make_matrix(vector<M, T> const &c0, vector<M, T> const &c1) noexcept {
    auto ret = matrix<2, M, T>{};
    ret[0] = c0;
    ret[1] = c1;
    return ret;
  }

  template<size_t M, typename T>
  constexpr auto make_matrix(
      vector<M, T> const &c0,
      vector<M, T> const &c1,
      vector<M, T> const &c2) noexcept {
    auto ret = matrix<3, M, T>{};
    ret[0] = c0;
    ret[1] = c1;
    ret[2] = c2;
    return ret;
  }

  template<size_t M, typename T>
  constexpr auto make_matrix(
      vector<M, T> const &c0,
      vector<M, T> const &c1,
      vector<M, T> const &c2,
      vector<M, T> const &c3) noexcept {
    auto ret = matrix<4, M, T>{};
    ret[0] = c0;
    ret[1] = c1;
    ret[2] = c2;
    ret[3] = c3;
    return ret;
  }

  template<size_t N, size_t M = N, typename T>
  constexpr auto zero_matrix() noexcept {
    return matrix<N, M, T>{};
  }

  template<size_t N, size_t M = N, typename T>
  constexpr auto identity_matrix() noexcept {
    auto ret = matrix<N, M, T>{};
    for (auto i = size_t{}; i < min(N, M); ++i)
      ret[i][i] = T{1};
    return ret;
  }

  template<typename T>
  constexpr auto scale_matrix3x3(T x) {
    auto ret = matrix3x3<T>{};
    ret[0][0] = x;
    ret[1][1] = x;
    ret[2][2] = x;
    return ret;
  }

  template<typename T>
  constexpr auto scale_matrix4x3(T x) {
    auto ret = matrix4x3<T>{};
    ret[0][0] = x;
    ret[1][1] = x;
    ret[2][2] = x;
    return ret;
  }

  template<typename T>
  constexpr auto scale_matrix4x4(T x) {
    auto ret = matrix4x3<T>{};
    ret[0][0] = x;
    ret[1][1] = x;
    ret[2][2] = x;
    ret[3][3] = T{1};
    return ret;
  }

  template<typename T>
  constexpr auto rotation_matrix3x3(quaternion<T> const &q) {
    auto wx = q.w() * q.x();
    auto wy = q.w() * q.y();
    auto wz = q.w() * q.z();
    auto x2 = q.x() * q.x();
    auto xy = q.x() * q.y();
    auto xz = q.x() * q.z();
    auto y2 = q.y() * q.y();
    auto yz = q.y() * q.z();
    auto z2 = q.z() * q.z();
    return make_matrix(
        make_vector(
            T{1} - T{2} * y2 - T{2} * z2,
            T{2} * xy + T{2} * wz,
            T{2} * xz - T{2} * wy),
        make_vector(
            T{2} * xy - T{2} * wz,
            T{1} - T{2} * x2 - T{2} * z2,
            T{2} * yz + T{2} * wx),
        make_vector(
            T{2} * xz + T{2} * wy,
            T{2} * yz - T{2} * wx,
            T{1} - T{2} * x2 - T{2} * y2));
  }

  template<typename T>
  constexpr auto rotation_matrix4x3(quaternion<T> const &q) {
    auto wx = q.w() * q.x();
    auto wy = q.w() * q.y();
    auto wz = q.w() * q.z();
    auto x2 = q.x() * q.x();
    auto xy = q.x() * q.y();
    auto xz = q.x() * q.z();
    auto y2 = q.y() * q.y();
    auto yz = q.y() * q.z();
    auto z2 = q.z() * q.z();
    return make_matrix(
        make_vector(
            T{1} - T{2} * y2 - T{2} * z2,
            T{2} * xy + T{2} * wz,
            T{2} * xz - T{2} * wy),
        make_vector(
            T{2} * xy - T{2} * wz,
            T{1} - T{2} * x2 - T{2} * z2,
            T{2} * yz + T{2} * wx),
        make_vector(
            T{2} * xz + T{2} * wy,
            T{2} * yz - T{2} * wx,
            T{1} - T{2} * x2 - T{2} * y2),
        zero_vector<3, T>());
  }

  template<typename T>
  constexpr auto rotation_matrix4x4(quaternion<T> const &q) {
    auto wx = q.w() * q.x();
    auto wy = q.w() * q.y();
    auto wz = q.w() * q.z();
    auto x2 = q.x() * q.x();
    auto xy = q.x() * q.y();
    auto xz = q.x() * q.z();
    auto y2 = q.y() * q.y();
    auto yz = q.y() * q.z();
    auto z2 = q.z() * q.z();
    return make_matrix(
        make_vector(
            T{1} - T{2} * y2 - T{2} * z2,
            T{2} * xy + T{2} * wz,
            T{2} * xz - T{2} * wy,
            T{}),
        make_vector(
            T{2} * xy - T{2} * wz,
            T{1} - T{2} * x2 - T{2} * z2,
            T{2} * yz + T{2} * wx,
            T{}),
        make_vector(
            T{2} * xz + T{2} * wy,
            T{2} * yz - T{2} * wx,
            T{1} - T{2} * x2 - T{2} * y2,
            T{}),
        make_vector(T{}, T{}, T{}, T{1}));
  }

  template<typename T>
  constexpr auto translation_matrix4x3(vector3<T> const &v) {
    return make_matrix(
        make_vector(T{1}, T{0}, T{0}),
        make_vector(T{0}, T{1}, T{0}),
        make_vector(T{0}, T{0}, T{1}),
        v);
  }

  template<typename T>
  constexpr auto translation_matrix4x4(vector3<T> const &v) {
    return make_matrix(
        make_vector(T{1}, T{0}, T{0}, T{0}),
        make_vector(T{0}, T{1}, T{0}, T{0}),
        make_vector(T{0}, T{0}, T{1}, T{0}),
        make_vector(v[0], v[1], v[2], T{1}));
  }

  template<typename T>
  constexpr auto perspective_matrix(T l, T r, T t, T b, T n, T f) {
    return make_matrix(
        make_vector(T{2} * n / (r - l), T{0}, T{0}, T{0}),
        make_vector(T{0}, T{2} * n / (b - t), T{0}, T{0}),
        make_vector((r + l) / (r - l), (b + t) / (b - t), n / (f - n), T{-1}),
        make_vector(T{0}, T{0}, n * f / (f - n), T{0}));
  }

  template<size_t N, size_t M, typename T>
  constexpr bool
  operator==(matrix<N, M, T> const &lhs, matrix<N, M, T> const &rhs) {
    for (auto i = size_t{}; i < N; ++i)
      if (lhs[i] != rhs[i])
        return false;
    return true;
  }

  template<size_t N, size_t M, typename T>
  constexpr bool
  operator!=(matrix<N, M, T> const &lhs, matrix<N, M, T> const &rhs) {
    return !(lhs == rhs);
  }

  template<size_t N, size_t M, typename T>
  constexpr auto transpose(matrix<N, M, T> const &m) {
    auto ret = matrix<T, M, N>{};
    for (auto i = size_t{}; i < M; ++i)
      for (auto j = size_t{}; j < N; ++j)
        ret[i][j] = m[j][i];
    return ret;
  }

  template<size_t N, size_t M, typename T>
  constexpr auto operator+(matrix<N, M, T> const &m) noexcept {
    return m;
  }

  template<size_t N, size_t M, typename T>
  constexpr auto operator-(matrix<N, M, T> const &m) noexcept {
    auto ret = matrix<N, M, decltype(-m[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = -m[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator*(T lhs, matrix<N, M, U> const &rhs) noexcept {
    auto ret = matrix<N, M, decltype(lhs * rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator*(matrix<N, M, T> const &lhs, U rhs) noexcept {
    auto ret = matrix<N, M, decltype(lhs[0][0] * rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] * rhs;
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &operator*=(matrix<N, M, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] *= rhs;
    return lhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator*(matrix<N, M, T> const &lhs, vector<N, U> const &rhs) noexcept {
    auto ret = lhs[0] * rhs[0];
    for (auto i = size_t{1}; i < N; ++i)
      ret += lhs[i] * rhs[i];
    return ret;
  }

  template<size_t P, size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator*(matrix<N, M, T> const &lhs, matrix<P, N, U> const &rhs) noexcept {
    auto ret = matrix<P, M, decltype(lhs[0][0] * rhs[0][0])>{};
    for (auto i = size_t{}; i < P; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &
  operator*=(matrix<N, M, T> &lhs, matrix<N, N, U> const &rhs) noexcept {
    return lhs = lhs * rhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator/(T lhs, matrix<N, M, U> const &rhs) noexcept {
    auto ret = matrix<M, N, decltype(lhs / rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs / rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator/(matrix<N, M, T> const &lhs, U rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>)
      return lhs * (T{1} / rhs);
    else if constexpr (std::is_floating_point_v<U>)
      return lhs * (U{1} / rhs);
    else {
      auto ret = matrix<N, M, decltype(lhs[0][0] / rhs)>{};
      for (auto i = size_t{}; i < N; ++i)
        ret[i] = lhs[i] / rhs;
      return ret;
    }
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &operator/=(matrix<N, M, T> &lhs, U rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>)
      return lhs *= (T{1} / rhs);
    else if constexpr (std::is_floating_point_v<U>)
      return lhs *= (U{1} / rhs);
    else {
      for (auto i = size_t{}; i < N; ++i)
        lhs[i] /= rhs;
      return lhs;
    }
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator+(T lhs, matrix<N, M, U> const &rhs) noexcept {
    auto ret = matrix<N, M, decltype(lhs + rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs + rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator+(matrix<N, M, T> const &lhs, U rhs) noexcept {
    auto ret = matrix<N, M, decltype(lhs[0][0] + rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs;
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator+(matrix<N, M, T> const &lhs, matrix<N, M, U> const &rhs) noexcept {
    auto ret = matrix<N, M, decltype(lhs[0][0] + rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &
  operator+=(matrix<N, M, T> &lhs, matrix<N, M, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] += rhs[i];
    return lhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator-(T lhs, matrix<N, M, U> const &rhs) noexcept {
    auto ret = matrix<M, N, decltype(lhs - rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs - rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator-(matrix<N, M, T> const &lhs, U rhs) noexcept {
    auto ret = matrix<M, N, decltype(lhs[0][0] - rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs;
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &operator-=(matrix<N, M, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs;
    return lhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator-(matrix<N, M, T> const &lhs, matrix<N, M, U> const &rhs) noexcept {
    auto ret = matrix<N, M, decltype(lhs[0][0] - rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &
  operator-=(matrix<N, M, T> &lhs, matrix<N, M, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs[i];
    return lhs;
  }

  template<size_t N, typename T>
  constexpr auto inverse(matrix<N, N, T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto augL = m;
    auto augR = identity_matrix<N, N, T>();
    for (auto j = size_t{}; j < N; ++j) {
      auto i = j;
      for (auto currI = i + 1; currI < N; ++currI)
        if (abs(augL[j][currI]) > abs(augL[j][i]))
          i = currI;
      if (!augL[j][i])
        throw std::runtime_error{"m is not invertible"};
      if (i != j) {
        for (auto &col : augL)
          std::swap(col[i], col[j]);
        for (auto &col : augR)
          std::swap(col[i], col[j]);
      }
      auto scalar = T{1} / augL[j][j];
      for (auto &col : augL)
        col[j] *= scalar;
      for (auto &col : augR)
        col[j] *= scalar;
      for (auto r = size_t{}; r < j; ++r) {
        for (auto &col : augL)
          col[r] -= augL[j][r] * col[j];
        for (auto &col : augR)
          col[r] -= augL[j][r] * col[j];
      }
      for (auto r = j + 1; r < N; ++r) {
        for (auto &col : augL)
          col[r] -= augL[j][r] * col[j];
        for (auto &col : augR)
          col[r] -= augL[j][r] * col[j];
      }
    }
    return augR;
  }

  template<typename T>
  constexpr auto determinant(matrix1x1<T> const &m) noexcept {
    return m[0][0];
  }

  template<typename T>
  constexpr auto determinant(matrix2x2<T> const &m) noexcept {
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
  }

  template<typename T>
  constexpr auto determinant(matrix3x3<T> const &m) noexcept {
    return m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
           m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) +
           m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
  }

  template<typename T>
  constexpr auto inverse(matrix1x1<T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto det = determinant(m);
    if (!det)
      throw std::runtime_error{"m is not invertible"};
    return make_matrix(make_vector(T{1})) / det;
  }

  template<typename T>
  constexpr auto inverse(matrix2x2<T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto det = determinant(m);
    if (!det)
      throw std::runtime_error{"m is not invertible"};
    return make_matrix(
               make_vector(m[1][1], -m[0][1]), make_vector(-m[1][0], m[0][0])) /
           det;
  }

  template<typename T>
  constexpr auto inverse(matrix3x3<T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto det = determinant(m);
    if (!det)
      throw std::runtime_error{"m is not invertible"};
    return make_matrix(
               make_vector(
                   m[1][1] * m[2][2] - m[2][1] * m[1][2],
                   m[2][1] * m[0][2] - m[0][1] * m[2][2],
                   m[0][1] * m[2][1] - m[1][1] * m[0][2]),
               make_vector(
                   m[2][0] * m[1][2] - m[1][0] * m[2][2],
                   m[0][0] * m[2][2] - m[2][0] * m[0][2],
                   m[1][0] * m[0][2] - m[0][0] * m[1][2]),
               make_vector(
                   m[1][0] * m[2][1] - m[2][0] * m[1][1],
                   m[2][0] * m[0][1] - m[0][0] * m[2][1],
                   m[0][0] * m[1][1] - m[1][0] * m[0][1])) /
           det;
  }

  template<typename T, size_t N, size_t M>
  auto &operator<<(std::ostream &os, matrix<N, M, T> const &m) {
    os << "[";
    for (auto i = size_t{0}; i < N; ++i) {
      for (auto j = size_t{0}; j < M; ++j) {
        os << m[i][j];
        if (j + 1 != M)
          os << ", ";
      }
      if (i + 1 != N)
        os << "; ";
    }
    return os << "]";
  }
} // namespace imp
