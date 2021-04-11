#include "spot_light_component.h"

namespace imp {
  vector3f const &spot_light_component::get_color() const noexcept {
    return color_;
  }

  float spot_light_component::get_cos_inner() const noexcept {
    return cos_inner_;
  }

  float spot_light_component::get_cos_outer() const noexcept {
    return cos_outer_;
  }

  void spot_light_component::set_color(vector3f const &color) noexcept {
    color_ = color;
  }

  void spot_light_component::set_cos_inner(float cos_inner) noexcept {
    cos_inner_ = cos_inner;
  }

  void spot_light_component::set_cos_outer(float cos_outer) noexcept {
    cos_outer_ = cos_outer;
  }
} // namespace imp