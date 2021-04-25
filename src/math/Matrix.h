#pragma once

#include <stdexcept>
#include <utility>

#include "Quaternion.h"
#include "Vector.h"

namespace imp {
  /**
   * @brief an NxM (n columns, m rows) matrix.
   */
  template<std::size_t N, std::size_t M, typename T>
  class Matrix {
  public:
    using Column = Vector<M, T>;
    using Scalar = T;

    constexpr Matrix() noexcept: cols_{} {}

    template<typename U>
    constexpr Matrix(Matrix<N, M, U> const &rhs) noexcept: cols_{} {
      for (auto i = size_t{}; i < N; ++i)
        cols_[i] = rhs[i];
    }

    template<typename U>
    constexpr Matrix &operator=(Matrix<N, M, U> const &rhs) noexcept {
      for (auto i = size_t{}; i < N; ++i)
        cols_[i] = rhs[i];
    }

    constexpr std::size_t cols() const noexcept {
      return N;
    }

    constexpr std::size_t rows() const noexcept {
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
    std::array<Column, N> cols_;
  };

  template<std::size_t M, typename T>
  class Matrix<1, M, T> {
  public:
    using Column = Vector<M, T>;
    using Scalar = T;

    constexpr Matrix() noexcept: cols_{} {}

    constexpr Matrix(Vector<M, T> const &i) noexcept: cols_{i} {}

    template<typename U>
    constexpr Matrix(Matrix<1, M, U> const &rhs) noexcept: cols_{rhs[0]} {}

    template<typename U>
    constexpr Matrix &operator=(Matrix<1, M, U> const &rhs) noexcept {
      cols_[0] = rhs[0];
    }

    constexpr std::size_t cols() const noexcept {
      return 1;
    }

    constexpr std::size_t rows() const noexcept {
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
    std::array<Vector<M, T>, 1> cols_;
  };

  template<std::size_t M, typename T>
  class Matrix<2, M, T> {
  public:
    using Column = Vector<M, T>;
    using Scalar = T;

    constexpr Matrix() noexcept: cols_{} {}

    constexpr Matrix(Vector<M, T> const &i) noexcept: cols_{i, i} {}

    constexpr Matrix(Vector<M, T> const &i, Vector<M, T> const &j) noexcept:
        cols_{i, j} {}

    template<typename U>
    constexpr Matrix(Matrix<2, M, U> const &rhs) noexcept:
        cols_{rhs[0], rhs[1]} {}

    template<typename U>
    constexpr Matrix &operator=(Matrix<2, M, U> const &rhs) noexcept {
      cols_[0] = rhs[0];
      cols_[1] = rhs[1];
    }

    constexpr std::size_t cols() const noexcept {
      return 2;
    }

    constexpr std::size_t rows() const noexcept {
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
    std::array<Vector<M, T>, 2> cols_;
  };

  template<std::size_t M, typename T>
  class Matrix<3, M, T> {
  public:
    using Column = Vector<M, T>;
    using Scalar = T;

    constexpr Matrix() noexcept: cols_{} {}

    constexpr Matrix(Vector<M, T> const &i) noexcept: cols_{i, i, i} {}

    constexpr Matrix(
        Vector<M, T> const &i,
        Vector<M, T> const &j,
        Vector<M, T> const &k) noexcept:
        cols_{i, j, k} {}

    template<typename U>
    constexpr Matrix(Matrix<3, M, U> const &rhs) noexcept:
        cols_{rhs[0], rhs[1], rhs[2]} {}

    template<typename U>
    constexpr Matrix &operator=(Matrix<3, M, U> const &rhs) noexcept {
      cols_[0] = rhs[0];
      cols_[1] = rhs[1];
      cols_[2] = rhs[2];
    }

    constexpr std::size_t cols() const noexcept {
      return 3;
    }

    constexpr std::size_t rows() const noexcept {
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
    std::array<Vector<M, T>, 3> cols_;
  };

  template<std::size_t M, typename T>
  class Matrix<4, M, T> {
  public:
    using Column = Vector<M, T>;
    using Scalar = T;

    constexpr Matrix() noexcept: cols_{} {}

    constexpr Matrix(Vector<M, T> const &i) noexcept: cols_{i, i, i, i} {}

    constexpr Matrix(
        Vector<M, T> const &i,
        Vector<M, T> const &j,
        Vector<M, T> const &k,
        Vector<M, T> const &l) noexcept:
        cols_{i, j, k, l} {}

    template<typename U>
    constexpr Matrix(Matrix<4, M, U> const &rhs) noexcept:
        cols_{rhs[0], rhs[1], rhs[2], rhs[3]} {}

    template<typename U>
    constexpr Matrix &operator=(Matrix<4, M, U> const &rhs) noexcept {
      cols_[0] = rhs[0];
      cols_[1] = rhs[1];
      cols_[2] = rhs[2];
      cols_[3] = rhs[3];
    }

    constexpr std::size_t cols() const noexcept {
      return 4;
    }

    constexpr std::size_t rows() const noexcept {
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
    std::array<Vector<M, T>, 4> cols_;
  };

  template<typename T>
  using Matrix1x1 = Matrix<1, 1, T>;

  template<typename T>
  using Matrix1x2 = Matrix<1, 2, T>;

  template<typename T>
  using Matrix1x3 = Matrix<1, 3, T>;

  template<typename T>
  using Matrix1x4 = Matrix<1, 4, T>;

  template<typename T>
  using Matrix2x1 = Matrix<2, 1, T>;

  template<typename T>
  using Matrix2x2 = Matrix<2, 2, T>;

  template<typename T>
  using Matrix2x3 = Matrix<2, 3, T>;

  template<typename T>
  using Matrix2x4 = Matrix<2, 4, T>;

  template<typename T>
  using Matrix3x1 = Matrix<3, 1, T>;

  template<typename T>
  using Matrix3x2 = Matrix<3, 2, T>;

  template<typename T>
  using Matrix3x3 = Matrix<3, 3, T>;

  template<typename T>
  using Matrix3x4 = Matrix<3, 4, T>;

  template<typename T>
  using Matrix4x1 = Matrix<4, 1, T>;

  template<typename T>
  using Matrix4x2 = Matrix<4, 2, T>;

  template<typename T>
  using Matrix4x3 = Matrix<4, 3, T>;

  template<typename T>
  using Matrix4x4 = Matrix<4, 4, T>;

  using Matrix1x1f = Matrix1x1<float>;
  using Matrix1x2f = Matrix1x2<float>;
  using Matrix1x3f = Matrix1x2<float>;
  using Matrix1x4f = Matrix1x4<float>;
  using Matrix2x1f = Matrix2x1<float>;
  using Matrix2x2f = Matrix2x2<float>;
  using Matrix2x3f = Matrix2x3<float>;
  using Matrix2x4f = Matrix2x4<float>;
  using Matrix3x1f = Matrix3x1<float>;
  using Matrix3x2f = Matrix3x2<float>;
  using Matrix3x3f = Matrix3x3<float>;
  using Matrix4x1f = Matrix4x1<float>;
  using Matrix4x2f = Matrix4x2<float>;
  using Matrix4x3f = Matrix4x3<float>;
  using Matrix4x4f = Matrix4x4<float>;
  using Matrix1x1i = Matrix1x1<std::int32_t>;
  using Matrix1x2i = Matrix1x2<std::int32_t>;
  using Matrix1x3i = Matrix1x2<std::int32_t>;
  using Matrix1x4i = Matrix1x4<std::int32_t>;
  using Matrix2x1i = Matrix2x1<std::int32_t>;
  using Matrix2x2i = Matrix2x2<std::int32_t>;
  using Matrix2x3i = Matrix2x3<std::int32_t>;
  using Matrix2x4i = Matrix2x4<std::int32_t>;
  using Matrix3x1i = Matrix3x1<std::int32_t>;
  using Matrix3x2i = Matrix3x2<std::int32_t>;
  using Matrix3x3i = Matrix3x3<std::int32_t>;
  using Matrix4x1i = Matrix4x1<std::int32_t>;
  using Matrix4x2i = Matrix4x2<std::int32_t>;
  using Matrix4x3i = Matrix4x3<std::int32_t>;
  using Matrix4x4i = Matrix4x4<std::int32_t>;
  using Matrix1x1u = Matrix1x1<std::uint32_t>;
  using Matrix1x2u = Matrix1x2<std::uint32_t>;
  using Matrix1x3u = Matrix1x2<std::uint32_t>;
  using Matrix1x4u = Matrix1x4<std::uint32_t>;
  using Matrix2x1u = Matrix2x1<std::uint32_t>;
  using Matrix2x2u = Matrix2x2<std::uint32_t>;
  using Matrix2x3u = Matrix2x3<std::uint32_t>;
  using Matrix2x4u = Matrix2x4<std::uint32_t>;
  using Matrix3x1u = Matrix3x1<std::uint32_t>;
  using Matrix3x2u = Matrix3x2<std::uint32_t>;
  using Matrix3x3u = Matrix3x3<std::uint32_t>;
  using Matrix4x1u = Matrix4x1<std::uint32_t>;
  using Matrix4x2u = Matrix4x2<std::uint32_t>;
  using Matrix4x3u = Matrix4x3<std::uint32_t>;
  using Matrix4x4u = Matrix4x4<std::uint32_t>;

  template<size_t N, size_t M = N, typename T>
  constexpr auto identityMatrix() noexcept {
    auto ret = Matrix<N, M, T>{};
    for (auto i = size_t{}; i < min(N, M); ++i)
      ret[i][i] = T{1};
    return ret;
  }

  template<typename T>
  constexpr auto scaleMatrix3x3(T x) {
    auto ret = Matrix3x3<T>{};
    ret[0][0] = x;
    ret[1][1] = x;
    ret[2][2] = x;
    return ret;
  }

  template<typename T>
  constexpr auto scaleMatrix4x3(T x) {
    auto ret = Matrix4x3<T>{};
    ret[0][0] = x;
    ret[1][1] = x;
    ret[2][2] = x;
    return ret;
  }

  template<typename T>
  constexpr auto scaleMatrix4x4(T x) {
    auto ret = Matrix4x3<T>{};
    ret[0][0] = x;
    ret[1][1] = x;
    ret[2][2] = x;
    ret[3][3] = T{1};
    return ret;
  }

  template<typename T>
  constexpr auto rotationMatrix3x3(Quaternion<T> const &q) {
    auto wx = q.s * q.v[0];
    auto wy = q.s * q.v[1];
    auto wz = q.s * q.v[2];
    auto x2 = q.v[0] * q.v[0];
    auto xy = q.v[0] * q.v[1];
    auto xz = q.v[0] * q.v[2];
    auto y2 = q.v[1] * q.v[1];
    auto yz = q.v[1] * q.v[2];
    auto z2 = q.v[2] * q.v[2];
    return Matrix3x3<T>(
        Vector3<T>{
            T{1} - T{2} * y2 - T{2} * z2,
            T{2} * xy + T{2} * wz,
            T{2} * xz - T{2} * wy},
        Vector3<T>{
            T{2} * xy - T{2} * wz,
            T{1} - T{2} * x2 - T{2} * z2,
            T{2} * yz + T{2} * wx},
        Vector3<T>{
            T{2} * xz + T{2} * wy,
            T{2} * yz - T{2} * wx,
            T{1} - T{2} * x2 - T{2} * y2});
  }

  template<typename T>
  constexpr auto rotationMatrix4x3(Quaternion<T> const &q) {
    auto wx = q.s * q.v[0];
    auto wy = q.s * q.v[1];
    auto wz = q.s * q.v[2];
    auto x2 = q.v[0] * q.v[0];
    auto xy = q.v[0] * q.v[1];
    auto xz = q.v[0] * q.v[2];
    auto y2 = q.v[1] * q.v[1];
    auto yz = q.v[1] * q.v[2];
    auto z2 = q.v[2] * q.v[2];
    return Matrix4x3<T>{
        Vector3<T>{
            T{1} - T{2} * y2 - T{2} * z2,
            T{2} * xy + T{2} * wz,
            T{2} * xz - T{2} * wy},
        Vector3<T>{
            T{2} * xy - T{2} * wz,
            T{1} - T{2} * x2 - T{2} * z2,
            T{2} * yz + T{2} * wx},
        Vector3<T>{
            T{2} * xz + T{2} * wy,
            T{2} * yz - T{2} * wx,
            T{1} - T{2} * x2 - T{2} * y2},
        Vector3<T>{}};
  }

  template<typename T>
  constexpr auto rotationMatrix4x4(Quaternion<T> const &q) {
    auto wx = q.s * q.v[0];
    auto wy = q.s * q.v[1];
    auto wz = q.s * q.v[2];
    auto x2 = q.v[0] * q.v[0];
    auto xy = q.v[0] * q.v[1];
    auto xz = q.v[0] * q.v[2];
    auto y2 = q.v[1] * q.v[1];
    auto yz = q.v[1] * q.v[2];
    auto z2 = q.v[2] * q.v[2];
    return Matrix4x4<T>{
        Vector4<T>{
            T{1} - T{2} * y2 - T{2} * z2,
            T{2} * xy + T{2} * wz,
            T{2} * xz - T{2} * wy,
            T{}},
        Vector4<T>{
            T{2} * xy - T{2} * wz,
            T{1} - T{2} * x2 - T{2} * z2,
            T{2} * yz + T{2} * wx,
            T{}},
        Vector4<T>{
            T{2} * xz + T{2} * wy,
            T{2} * yz - T{2} * wx,
            T{1} - T{2} * x2 - T{2} * y2,
            T{}},
        Vector4<T>{T{}, T{}, T{}, T{1}}};
  }

  template<typename T>
  constexpr auto translationMatrix4x3(Vector3<T> const &v) {
    return Matrix4x3<T>{
        Vector3<T>{T{1}, T{0}, T{0}},
        Vector3<T>{T{0}, T{1}, T{0}},
        Vector3<T>{T{0}, T{0}, T{1}},
        v};
  }

  template<typename T>
  constexpr auto translationMatrix4x4(Vector3<T> const &v) {
    return Matrix4x4<T>{
        Vector4<T>{T{1}, T{0}, T{0}, T{0}},
        Vector4<T>{T{0}, T{1}, T{0}, T{0}},
        Vector4<T>{T{0}, T{0}, T{1}, T{0}},
        Vector4<T>{v[0], v[1], v[2], T{1}}};
  }

  template<typename T>
  constexpr auto perspectiveMatrix(T l, T r, T t, T b, T n, T f) {
    return Matrix4x4<T>{
        Vector4<T>{T{2} * n / (r - l), T{0}, T{0}, T{0}},
        Vector4<T>{T{0}, T{2} * n / (b - t), T{0}, T{0}},
        Vector4<T>{(r + l) / (r - l), (b + t) / (b - t), n / (f - n), T{-1}},
        Vector4<T>{T{0}, T{0}, n * f / (f - n), T{0}}};
  }

  template<size_t N, size_t M, typename T>
  constexpr bool
  operator==(Matrix<N, M, T> const &lhs, Matrix<N, M, T> const &rhs) {
    for (auto i = size_t{}; i < N; ++i)
      if (lhs[i] != rhs[i])
        return false;
    return true;
  }

  template<size_t N, size_t M, typename T>
  constexpr bool
  operator!=(Matrix<N, M, T> const &lhs, Matrix<N, M, T> const &rhs) {
    return !(lhs == rhs);
  }

  template<size_t N, size_t M, typename T>
  constexpr auto transpose(Matrix<N, M, T> const &m) {
    auto ret = Matrix<T, M, N>{};
    for (auto i = size_t{}; i < M; ++i)
      for (auto j = size_t{}; j < N; ++j)
        ret[i][j] = m[j][i];
    return ret;
  }

  template<size_t N, size_t M, typename T>
  constexpr auto operator+(Matrix<N, M, T> const &m) noexcept {
    return m;
  }

  template<size_t N, size_t M, typename T>
  constexpr auto operator-(Matrix<N, M, T> const &m) noexcept {
    auto ret = Matrix<N, M, decltype(-m[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = -m[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator*(T lhs, Matrix<N, M, U> const &rhs) noexcept {
    auto ret = Matrix<N, M, decltype(lhs * rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator*(Matrix<N, M, T> const &lhs, U rhs) noexcept {
    auto ret = Matrix<N, M, decltype(lhs[0][0] * rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] * rhs;
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &operator*=(Matrix<N, M, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] *= rhs;
    return lhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator*(Matrix<N, M, T> const &lhs, Vector<N, U> const &rhs) noexcept {
    auto ret = lhs[0] * rhs[0];
    for (auto i = size_t{1}; i < N; ++i)
      ret += lhs[i] * rhs[i];
    return ret;
  }

  template<size_t P, size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator*(Matrix<N, M, T> const &lhs, Matrix<P, N, U> const &rhs) noexcept {
    auto ret = Matrix<P, M, decltype(lhs[0][0] * rhs[0][0])>{};
    for (auto i = size_t{}; i < P; ++i)
      ret[i] = lhs * rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &
  operator*=(Matrix<N, M, T> &lhs, Matrix<N, N, U> const &rhs) noexcept {
    return lhs = lhs * rhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator/(T lhs, Matrix<N, M, U> const &rhs) noexcept {
    auto ret = Matrix<M, N, decltype(lhs / rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs / rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator/(Matrix<N, M, T> const &lhs, U rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>)
      return lhs * (T{1} / rhs);
    else if constexpr (std::is_floating_point_v<U>)
      return lhs * (U{1} / rhs);
    else {
      auto ret = Matrix<N, M, decltype(lhs[0][0] / rhs)>{};
      for (auto i = size_t{}; i < N; ++i)
        ret[i] = lhs[i] / rhs;
      return ret;
    }
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &operator/=(Matrix<N, M, T> &lhs, U rhs) noexcept {
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
  constexpr auto operator+(T lhs, Matrix<N, M, U> const &rhs) noexcept {
    auto ret = Matrix<N, M, decltype(lhs + rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs + rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator+(Matrix<N, M, T> const &lhs, U rhs) noexcept {
    auto ret = Matrix<N, M, decltype(lhs[0][0] + rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs;
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator+(Matrix<N, M, T> const &lhs, Matrix<N, M, U> const &rhs) noexcept {
    auto ret = Matrix<N, M, decltype(lhs[0][0] + rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] + rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &
  operator+=(Matrix<N, M, T> &lhs, Matrix<N, M, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] += rhs[i];
    return lhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator-(T lhs, Matrix<N, M, U> const &rhs) noexcept {
    auto ret = Matrix<M, N, decltype(lhs - rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs - rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto operator-(Matrix<N, M, T> const &lhs, U rhs) noexcept {
    auto ret = Matrix<M, N, decltype(lhs[0][0] - rhs)>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs;
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &operator-=(Matrix<N, M, T> &lhs, U rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs;
    return lhs;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto
  operator-(Matrix<N, M, T> const &lhs, Matrix<N, M, U> const &rhs) noexcept {
    auto ret = Matrix<N, M, decltype(lhs[0][0] - rhs[0][0])>{};
    for (auto i = size_t{}; i < N; ++i)
      ret[i] = lhs[i] - rhs[i];
    return ret;
  }

  template<size_t N, size_t M, typename T, typename U>
  constexpr auto &
  operator-=(Matrix<N, M, T> &lhs, Matrix<N, M, U> const &rhs) noexcept {
    for (auto i = size_t{}; i < N; ++i)
      lhs[i] -= rhs[i];
    return lhs;
  }

  template<size_t N, typename T>
  constexpr auto inverse(Matrix<N, N, T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto augL = m;
    auto augR = identityMatrix<N, N, T>();
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
  constexpr auto determinant(Matrix1x1<T> const &m) noexcept {
    return m[0][0];
  }

  template<typename T>
  constexpr auto determinant(Matrix2x2<T> const &m) noexcept {
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
  }

  template<typename T>
  constexpr auto determinant(Matrix3x3<T> const &m) noexcept {
    return m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
           m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) +
           m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
  }

  template<typename T>
  constexpr auto inverse(Matrix1x1<T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto det = determinant(m);
    if (!det)
      throw std::runtime_error{"m is not invertible"};
    return makeMatrix(makeVector(T{1})) / det;
  }

  template<typename T>
  constexpr auto inverse(Matrix2x2<T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto det = determinant(m);
    if (!det)
      throw std::runtime_error{"m is not invertible"};
    return makeMatrix(
               makeVector(m[1][1], -m[0][1]), makeVector(-m[1][0], m[0][0])) /
           det;
  }

  template<typename T>
  constexpr auto inverse(Matrix3x3<T> const &m) {
    static_assert(std::is_floating_point_v<T>);
    auto det = determinant(m);
    if (!det)
      throw std::runtime_error{"m is not invertible"};
    return makeMatrix(
               makeVector(
                   m[1][1] * m[2][2] - m[2][1] * m[1][2],
                   m[2][1] * m[0][2] - m[0][1] * m[2][2],
                   m[0][1] * m[2][1] - m[1][1] * m[0][2]),
               makeVector(
                   m[2][0] * m[1][2] - m[1][0] * m[2][2],
                   m[0][0] * m[2][2] - m[2][0] * m[0][2],
                   m[1][0] * m[0][2] - m[0][0] * m[1][2]),
               makeVector(
                   m[1][0] * m[2][1] - m[2][0] * m[1][1],
                   m[2][0] * m[0][1] - m[0][0] * m[2][1],
                   m[0][0] * m[1][1] - m[1][0] * m[0][1])) /
           det;
  }

  template<typename T, size_t N, size_t M>
  auto &operator<<(std::ostream &os, Matrix<N, M, T> const &m) {
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
