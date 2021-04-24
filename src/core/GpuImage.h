#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.h"

namespace imp {
  class GpuImage {
  public:
    explicit GpuImage(
        vk::ImageCreateInfo const &imageCreateInfo,
        VmaAllocationCreateInfo const &allocationCreateInfo,
        VmaAllocator allocator);
    ~GpuImage();

    GpuImage(GpuImage &&rhs) noexcept;
    GpuImage &operator=(GpuImage &&rhs) noexcept;

    void reset() noexcept;

    vk::Image const *operator->() const noexcept {
      return &image_;
    }

    vk::Image const &operator*() const noexcept {
      return image_;
    }

    operator bool() const noexcept {
      return image_;
    }

  private:
    vk::Image image_;
    VmaAllocation allocation_;
    VmaAllocationInfo allocationInfo_;
    VmaAllocator allocator_;
  };
} // namespace imp