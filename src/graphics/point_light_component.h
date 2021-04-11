#pragma once

#include "../math/vector.h"

namespace imp {
  class point_light_component {
  public:
    vector3f const &get_color() const noexcept;
    void set_color(vector3f const &color) noexcept;

  private:
    vector3f color_ = 1.0f;
  };
} // namespace imp