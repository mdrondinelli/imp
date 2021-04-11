#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.h"

namespace imp {
  class gpu_image {
  public:
    gpu_image(
        vk::ImageCreateInfo const &image_info,
        VmaAllocationCreateInfo const &allocation_info,
        VmaAllocator allocator);
    ~gpu_image();

    gpu_image(gpu_image &&rhs) noexcept;
    gpu_image &operator=(gpu_image &&rhs) noexcept;

    vk::Image get() const noexcept;

  private:
    vk::Image image_;
    VmaAllocation allocation_;
    VmaAllocator allocator_;
  };
} // namespace imp