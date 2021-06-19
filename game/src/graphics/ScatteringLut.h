#pragma once

#include "../core/GpuContext.h"
#include "../math/Vector.h"

namespace imp {
  struct ScatteringLutCreateInfo {
    GpuContext *context;
    Vector3u size;
  };

  class ScatteringLut {
  public:
    explicit ScatteringLut(ScatteringLutCreateInfo const &createInfo);
    Vector3u const &getSize() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;

  private:
    Vector3u size_;
    GpuImage image_;
    vk::UniqueImageView imageView_;

    GpuImage createImage(GpuContext &context);
    vk::UniqueImageView createImageView(GpuContext &context);
  };
} // namespace imp