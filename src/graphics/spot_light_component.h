#pragma once

#include "../math/vector.h"

namespace imp {
  class spot_light_component {
  public:
    vector3f const &get_color() const noexcept;
    float get_cos_inner() const noexcept;
    float get_cos_outer() const noexcept;
    void set_color(vector3f const &color) noexcept;
    void set_cos_inner(float cos_inner) noexcept;
    void set_cos_outer(float cos_outer) noexcept;

  private:
    vector3f color_ = 1.0f;
    float cos_inner_ = 1.0f;
    float cos_outer_ = 0.5f;
  };
} // namespace imp