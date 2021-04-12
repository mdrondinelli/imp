#pragma once

#include "../core/gpu_context.h"
#include "../math/vector.h"

namespace imp {
  class scattering_lut {
  public:
    scattering_lut(gpu_context &context, vector3u const &size);
    vector3u const &size() const noexcept;
    vk::Image image() const noexcept;
    vk::ImageView image_view() const noexcept;

  private:
    vector3u size_;
    gpu_image image_;
    vk::UniqueImageView image_view_;

    gpu_image create_image(gpu_context &context, vector3u const &size);
    vk::UniqueImageView create_image_view(gpu_context &context);
  };
} // namespace imp