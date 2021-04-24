#include "GpuImage.h"

namespace imp {
  GpuImage::GpuImage(
      vk::ImageCreateInfo const &image_info,
      VmaAllocationCreateInfo const &allocation_info,
      VmaAllocator allocator):
      allocator_{allocator} {
    if (vmaCreateImage(
            allocator_,
            reinterpret_cast<VkImageCreateInfo const *>(&image_info),
            &allocation_info,
            reinterpret_cast<VkImage *>(&image_),
            &allocation_,
            &allocationInfo_) != VK_SUCCESS) {
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
      allocationInfo_{rhs.allocationInfo_},
      allocator_{rhs.allocator_} {
    rhs.image_ = vk::Image{};
  }

  GpuImage &GpuImage::operator=(GpuImage &&rhs) noexcept {
    if (&rhs != this) {
      if (image_) {
        vmaDestroyImage(allocator_, image_, allocation_);
      }
      image_ = rhs.image_;
      allocation_ = rhs.allocation_;
      allocationInfo_ = rhs.allocationInfo_;
      allocator_ = rhs.allocator_;
      rhs.image_ = vk::Image{};
    }
    return *this;
  }

  void GpuImage::reset() noexcept {
    if (image_) {
      vmaDestroyImage(allocator_, image_, allocation_);
      image_ = nullptr;
    }
  }
} // namespace imp