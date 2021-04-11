#pragma once

#include "../core/gpu_allocator.h"
#include "scattering_lut_create_info.h"

namespace imp {
  class scattering_lut {
  public:
    scattering_lut(
        scattering_lut_create_info const &info, gpu_allocator &allocator);
    vector3u const &size() const noexcept;
    vk::Image image() const noexcept;
    vk::ImageView image_view() const noexcept;

  private:
    vector3u size_;
    gpu_image image_;
    vk::UniqueImageView image_view_;

    gpu_image create_image(
        scattering_lut_create_info const &info, gpu_allocator &allocator);
    vk::UniqueImageView create_image_view(
        scattering_lut_create_info const &info, gpu_allocator &allocator);
  };
} // namespace imp