#pragma once

#include "vector.h"

namespace imp {
  /**
   * @brief A quaternion.
   */
  template<typename T>
  class quaternion {
  public:
    T s;
    vector3<T> v;

    /**
     * @brief Constructs a zero quaternion.
     */
    constexpr quaternion() noexcept: s{}, v{} {}

    constexpr quaternion(T s, vector3<T> const &v) noexcept: s{s}, v{v} {}

    constexpr T const &w() const noexcept {
      return s;
    }

    constexpr T &w() noexcept {
      return s;
    }

    constexpr T const &x() const noexcept {
      return v.x();
    }

    constexpr T &x() noexcept {
      return v.x();
    }

    constexpr T const &y() const noexcept {
      return v.y();
    }

    constexpr T &y() noexcept {
      return v.y();
    }

    constexpr T const &z() const noexcept {
      return v.z();
    }

    constexpr T &z() noexcept {
      return v.z();
    }
  };

  template<typename T>
  constexpr bool
  operator==(quaternion<T> const &lhs, quaternion<T> const &rhs) noexcept {
    return lhs.s == rhs.s && lhs.v == rhs.v;
  }

  template<typename T>
  constexpr bool
  operator!=(quaternion<T> const &lhs, quaternion<T> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<typename T>
  constexpr auto operator+(quaternion<T> const &q) noexcept {
    return q;
  }

  template<typename T>
  constexpr auto operator-(quaternion<T> const &q) noexcept {
    return quaternion{-q.s, -q.v};
  }

  template<typename T>
  constexpr auto operator*(T lhs, quaternion<T> const &rhs) noexcept {
    return quaternion{lhs * rhs.s, lhs * rhs.v};
  }

  template<typename T>
  constexpr auto operator*(quaternion<T> const &lhs, T rhs) noexcept {
    return rhs * lhs;
  }

  template<typename T>
  constexpr auto &operator*=(quaternion<T> &lhs, T rhs) noexcept {
    return lhs = lhs * rhs;
  }

  template<typename T>
  constexpr auto
  operator*(quaternion<T> const &lhs, quaternion<T> const &rhs) noexcept {
    return quaternion{
        lhs.s * rhs.s - dot(lhs.v, rhs.v),
        lhs.s * rhs.v + rhs.s * lhs.v + cross(lhs.v, rhs.v)};
  }

  template<typename T>
  constexpr auto &
  operator*=(quaternion<T> &lhs, quaternion<T> const &rhs) noexcept {
    return lhs = lhs * rhs;
  }

  template<typename T>
  constexpr auto operator/(quaternion<T> const &lhs, T rhs) noexcept {
    if constexpr (std::is_floating_point_v<T>)
      return lhs * (T{1} / rhs);
    else
      return quaternion{lhs.s / rhs, lhs.v / rhs};
  }

  template<typename T>
  constexpr auto &operator/=(quaternion<T> &lhs, T rhs) noexcept {
    return lhs = lhs / rhs;
  }

  template<typename T>
  constexpr auto
  operator+(quaternion<T> const &lhs, quaternion<T> const &rhs) noexcept {
    return quaternion{lhs.s + rhs.s, lhs.v + rhs.v};
  }

  template<typename T>
  constexpr auto &
  operator+=(quaternion<T> &lhs, quaternion<T> const &rhs) noexcept {
    return lhs = lhs + rhs;
  }

  template<typename T>
  constexpr auto
  operator-(quaternion<T> lhs, quaternion<T> const &rhs) noexcept {
    return quaternion{lhs.s - rhs.s, lhs.v - rhs.v};
  }

  template<typename T>
  constexpr auto &
  operator-=(quaternion<T> &lhs, quaternion<T> const &rhs) noexcept {
    return lhs = lhs - rhs;
  }

  template<typename T>
  constexpr auto length2(quaternion<T> const &q) noexcept {
    return q.s * q.s + length2(q.v);
  }

  template<typename T>
  auto length(quaternion<T> const &q) noexcept {
    if (std::is_same_v<T, double>)
      return std::sqrt(length2(q));
    else
      return std::sqrtf(length2(q));
  }

  template<typename T>
  auto normalize(quaternion<T> const &q) noexcept {
    return q / length(q);
  }

  template<typename T>
  constexpr auto conjugate(quaternion<T> const &q) noexcept {
    return quaternion{q.s, -q.v};
  }

  template<typename T>
  constexpr auto inverse(quaternion<T> const &q) noexcept {
    return conjugate(q) / length2(q);
  }

  template<typename T>
  constexpr auto zero_quaternion() noexcept {
    return quaternion<T>{};
  }

  template<typename T>
  constexpr auto identity_quaternion() noexcept {
    return quaternion{T{1}, zero_vector<3, T>()};
  }

  template<typename T>
  auto rotation_quaternion(T angle, vector3<T> const &axis) noexcept {
    return quaternion{std::cos(angle / 2), axis * std::sin(angle / 2)};
  }

  template<typename T>
  auto rotate(vector3<T> const &p, quaternion<T> const &q) noexcept {
    return (q.s * q.s - length2(q.v)) * p +
           T{2} * (q.s * cross(q.v, p) + dot(q.v, p) * q.v);
  }

  template<typename T>
  constexpr auto
  lerp(quaternion<T> const &q1, quaternion<T> const &q2, T t) noexcept {
    return (T{1} - t) * q1 + t * q2;
  }

  template<typename T>
  constexpr auto lerp_shortest(
      quaternion<T> const &q1, quaternion<T> const &q2, T t) noexcept {
    return dot(q1, q2) >= 0 ? lerp(q1, q2, t) : lerp(-q1, q2, t);
  }

  template<typename T>
  auto &operator<<(std::ostream &os, quaternion<T> const &q) noexcept {
    os << q.w();
    os << (q.x() < 0 ? " - " : " + ") << std::abs(q.x()) << "i";
    os << (q.y() < 0 ? " - " : " + ") << std::abs(q.y()) << "j";
    os << (q.z() < 0 ? " - " : " + ") << std::abs(q.z()) << "k";
    return os;
  }

  using quaternionf = quaternion<float>;
} // namespace imp
