#include "GpuImage.h"

#include "GpuContext.h"

namespace imp {
  GpuImage::GpuImage(GpuContext &context, GpuImageCreateInfo const &createInfo):
      allocator_{context.getAllocator()}, image_{}, allocation_{} {
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
    if (allocator_) {
      vmaDestroyImage(allocator_, image_, allocation_);
    }
  }

  GpuImage::GpuImage(GpuImage &&rhs) noexcept:
      allocator_{rhs.allocator_},
      image_{rhs.image_},
      allocation_{rhs.allocation_} {
    rhs.allocator_ = nullptr;
  }

  GpuImage &GpuImage::operator=(GpuImage &&rhs) noexcept {
    if (&rhs != this) {
      if (allocator_) {
        vmaDestroyImage(allocator_, image_, allocation_);
      }
      allocator_ = rhs.allocator_;
      image_ = rhs.image_;
      allocation_ = rhs.allocation_;
      rhs.allocator_ = nullptr;
    }
    return *this;
  }

  vk::Image GpuImage::get() const noexcept {
    return image_;
  }
} // namespace imp