#include "point_light_component.h"

namespace imp {
  vector3f const &point_light_component::get_color() const noexcept {
    return color_;
  }

  void point_light_component::set_color(vector3f const &color) noexcept {
    color_ = color;
  }
} // namespace imp