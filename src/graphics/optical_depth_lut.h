#pragma once

#include "../core/gpu_context.h"
#include "../math/vector.h"

namespace imp {
  class optical_depth_lut {
  public:
    optical_depth_lut(gpu_context &context, vector2u const &size);
    vector2u const &size() const noexcept;
    vk::Image image() const noexcept;
    vk::ImageView image_view() const noexcept;

  private:
    vector2u size_;
    gpu_image image_;
    vk::UniqueImageView image_view_;

    gpu_image create_image(gpu_context &context, vector2u const &size);
    vk::UniqueImageView create_image_view(gpu_context &context);
  };
} // namespace imp