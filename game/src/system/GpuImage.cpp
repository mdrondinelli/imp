#include "GpuImage.h"

#include "GpuContext.h"

namespace imp {
  GpuImage::GpuImage(GpuContext &context, GpuImageCreateInfo const &createInfo):
      allocator_{context.getAllocator()} {
    if (vmaCreateImage(
            allocator_,
            reinterpret_cast<VkImageCreateInfo const *>(&createInfo.image),
            &createInfo.allocation,
            reinterpret_cast<VkImage *>(&image_),
            &allocation_,
            nullptr) != VK_SUCCESS) {
      throw std::runtime_error{"failed to create gpu image."};
    }
  }

  GpuImage::~GpuImage() {
    if (image_) {
      vmaDestroyImage(allocator_, image_, allocation_);
    }
  }

  GpuImage::GpuImage(GpuImage &&rhs) noexcept:
      image_{rhs.image_},
      allocation_{rhs.allocation_},
      allocator_{rhs.allocator_} {
    rhs.image_ = nullptr;
  }

  GpuImage &GpuImage::operator=(GpuImage &&rhs) noexcept {
    if (&rhs != this) {
      if (image_) {
        vmaDestroyImage(allocator_, image_, allocation_);
      }
      image_ = rhs.image_;
      allocation_ = rhs.allocation_;
      allocator_ = rhs.allocator_;
      rhs.image_ = nullptr;
    }
    return *this;
  }
} // namespace imp