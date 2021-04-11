#pragma once

#include "../core/gpu_allocator.h"
#include "optical_depth_lut_create_info.h"

namespace imp {
  class optical_depth_lut {
  public:
    optical_depth_lut(
        optical_depth_lut_create_info const &info, gpu_allocator &allocator);
    vector2u const &size() const noexcept;
    vk::Image image() const noexcept;
    vk::ImageView image_view() const noexcept;

  private:
    vector2u size_;
    gpu_image image_;
    vk::UniqueImageView image_view_;

    gpu_image create_image(
        optical_depth_lut_create_info const &info, gpu_allocator &allocator);
    vk::UniqueImageView create_image_view(
        optical_depth_lut_create_info const &info, gpu_allocator &allocator);
  };
} // namespace imp