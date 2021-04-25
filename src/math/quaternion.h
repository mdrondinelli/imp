#pragma once

#include "Vector.h"

namespace imp {
  /**
   * @brief A quaternion.
   */
  template<typename T>
  class Quaternion {
  public:
    T s;
    Vector3<T> v;

    /**
     * @brief Constructs a zero quaternion.
     */
    constexpr Quaternion() noexcept: s{}, v{} {}

    constexpr Quaternion(T s, Vector3<T> const &v) noexcept: s{s}, v{v} {}
  };

  using Quaternionf = Quaternion<float>;

  template<typename T>
  constexpr bool
  operator==(Quaternion<T> const &lhs, Quaternion<T> const &rhs) noexcept {
    return lhs.s == rhs.s && lhs.v == rhs.v;
  }

  template<typename T>
  constexpr bool
  operator!=(Quaternion<T> const &lhs, Quaternion<T> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<typename T>
  constexpr auto operator+(Quaternion<T> const &q) noexcept {
    return q;
  }

  template<typename T>
  constexpr auto operator-(Quaternion<T> const &q) noexcept {
    return Quaternion{-q.s, -q.v};
  }

  template<typename T>
  constexpr auto operator*(T lhs, Quaternion<T> const &rhs) noexcept {
    return Quaternion{lhs * rhs.s, lhs * rhs.v};
  }

  template<typename T>
  constexpr auto operator*(Quaternion<T> const &lhs, T rhs) noexcept {
    return rhs * lhs;
  }

  template<typename T>
  constexpr auto &operator*=(Quaternion<T> &lhs, T rhs) noexcept {
    return lhs = lhs * rhs;
  }

  template<typename T>
  constexpr auto
  operator*(Quaternion<T> const &lhs, Quaternion<T> const &rhs) noexcept {
    return Quaternion{
        lhs.s * rhs.s - dot(lhs.v, rhs.v),
        lhs.s * rhs.v + rhs.s * lhs.v + cross(lhs.v, rhs.v)};
  }

  template<typename T>
  constexpr auto &
  operator*=(Quaternion<T> &lhs, Quaternion<T> const &rhs) noexcept {
    return lhs = lhs * rhs;
  }

  template<typename T>
  constexpr auto operator/(Quaternion<T> const &lhs, T rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>)
      return lhs * (T{1} / rhs);
    else
      return Quaternion{lhs.s / rhs, lhs.v / rhs};
  }

  template<typename T>
  constexpr auto &operator/=(Quaternion<T> &lhs, T rhs) noexcept {
    return lhs = lhs / rhs;
  }

  template<typename T>
  constexpr auto
  operator+(Quaternion<T> const &lhs, Quaternion<T> const &rhs) noexcept {
    return Quaternion{lhs.s + rhs.s, lhs.v + rhs.v};
  }

  template<typename T>
  constexpr auto &
  operator+=(Quaternion<T> &lhs, Quaternion<T> const &rhs) noexcept {
    return lhs = lhs + rhs;
  }

  template<typename T>
  constexpr auto
  operator-(Quaternion<T> lhs, Quaternion<T> const &rhs) noexcept {
    return Quaternion{lhs.s - rhs.s, lhs.v - rhs.v};
  }

  template<typename T>
  constexpr auto &
  operator-=(Quaternion<T> &lhs, Quaternion<T> const &rhs) noexcept {
    return lhs = lhs - rhs;
  }

  template<typename T>
  constexpr auto length2(Quaternion<T> const &q) noexcept {
    return q.s * q.s + length2(q.v);
  }

  template<typename T>
  auto length(Quaternion<T> const &q) noexcept {
    return sqrt(length2(q));
  }

  template<typename T>
  auto normalize(Quaternion<T> const &q) noexcept {
    return q / length(q);
  }

  template<typename T>
  constexpr auto conjugate(Quaternion<T> const &q) noexcept {
    return Quaternion{q.s, -q.v};
  }

  template<typename T>
  constexpr auto inverse(Quaternion<T> const &q) noexcept {
    return conjugate(q) / length2(q);
  }

  template<typename T>
  constexpr auto identityQuaternion() noexcept {
    return Quaternion{T{1}, Vector3<T>{}};
  }

  template<typename T>
  auto rotationQuaternion(T angle, Vector3<T> const &axis) noexcept {
    return Quaternion{cos(angle / 2), axis * sin(angle / 2)};
  }

  template<typename T>
  auto rotate(Vector3<T> const &p, Quaternion<T> const &q) noexcept {
    return (q.s * q.s - length2(q.v)) * p +
           T{2} * (q.s * cross(q.v, p) + dot(q.v, p) * q.v);
  }

  template<typename T>
  constexpr auto
  lerp(Quaternion<T> const &q1, Quaternion<T> const &q2, T t) noexcept {
    return (T{1} - t) * q1 + t * q2;
  }

  template<typename T>
  constexpr auto lerp_shortest(
      Quaternion<T> const &q1, Quaternion<T> const &q2, T t) noexcept {
    return dot(q1, q2) >= 0 ? lerp(q1, q2, t) : lerp(-q1, q2, t);
  }

  template<typename T>
  auto &operator<<(std::ostream &os, Quaternion<T> const &q) noexcept {
    os << q.w();
    os << (q.x() < 0 ? " - " : " + ") << abs(q.x()) << "i";
    os << (q.y() < 0 ? " - " : " + ") << abs(q.y()) << "j";
    os << (q.z() < 0 ? " - " : " + ") << abs(q.z()) << "k";
    return os;
  }
} // namespace imp
