#pragma once

#include <optional>

#include "sphere.h"

namespace imp {
  struct ray {
    vector3f origin;
    vector3f direction;

    constexpr auto operator()(float t) const noexcept {
      return origin + t * direction;
    }
  };

  inline bool test_isect(ray const &r, sphere const &s) {
    auto l = s.center - r.origin;
    auto tca = dot(l, r.direction);
    auto d2 = length2(l) - tca * tca;
    auto r2 = s.radius * s.radius;
    return d2 <= r2;
  }

  inline std::optional<float> find_isect(ray const &r, sphere const &s) {
    auto l = s.center - r.origin;
    auto tca = dot(l, r.direction);
    auto d2 = length2(l) - tca * tca;
    auto r2 = s.radius * s.radius;
    if (d2 <= r2) {
      auto thc = std::sqrt(r2 - d2);
      auto t0 = tca - thc;
      auto t1 = tca + thc;
      if (t0 > t1)
        std::swap(t0, t1);
      if (t0 >= 0)
        return t0;
      if (t1 >= 0)
        return t1;
    }
    return std::nullopt;
  }
} // namespace imp