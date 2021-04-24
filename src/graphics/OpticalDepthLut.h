#pragma once

#include "../core/GpuContext.h"
#include "../math/Vector.h"

namespace imp {
  struct OpticalDepthLutCreateInfo {
    GpuContext *context;
    Vector2u size;
  };

  class OpticalDepthLut {
  public:
    explicit OpticalDepthLut(OpticalDepthLutCreateInfo const &createInfo);
    Vector2u const &getSize() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;

  private:
    Vector2u size_;
    GpuImage image_;
    vk::UniqueImageView imageView_;

    GpuImage createImage(GpuContext &context);
    vk::UniqueImageView createImageView(GpuContext &context);
  };
} // namespace imp