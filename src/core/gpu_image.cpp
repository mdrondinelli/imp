#include "gpu_image.h"

namespace imp {
  gpu_image::gpu_image(
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
            &allocation_info_) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create vulkan buffer."};
    }
  }

  gpu_image::~gpu_image() {
    if (image_) {
      vmaDestroyImage(allocator_, image_, allocation_);
    }
  }

  gpu_image::gpu_image(gpu_image &&rhs) noexcept:
      image_{rhs.image_},
      allocation_{rhs.allocation_},
      allocation_info_{rhs.allocation_info_},
      allocator_{rhs.allocator_} {
    rhs.image_ = vk::Image{};
  }

  gpu_image &gpu_image::operator=(gpu_image &&rhs) noexcept {
    if (&rhs != this) {
      if (image_) {
        vmaDestroyImage(allocator_, image_, allocation_);
      }
      image_ = rhs.image_;
      allocation_ = rhs.allocation_;
      allocation_info_ = rhs.allocation_info_;
      allocator_ = rhs.allocator_;
      rhs.image_ = vk::Image{};
    }
    return *this;
  }

  void gpu_image::reset() noexcept {
    if (image_) {
      vmaDestroyImage(allocator_, image_, allocation_);
      image_ = nullptr;
    }
  }
} // namespace imp