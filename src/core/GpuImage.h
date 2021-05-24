#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.h"

namespace imp {
  class GpuContext;

  struct GpuImageCreateInfo {
    vk::ImageCreateInfo image;
    VmaAllocationCreateInfo allocation;
  };

  class GpuImage {
  public:
    GpuImage(GpuContext &context, GpuImageCreateInfo const &createInfo);
    ~GpuImage();

    GpuImage(GpuImage &&rhs) noexcept;
    GpuImage &operator=(GpuImage &&rhs) noexcept;

    vk::Image get() const noexcept;

  private:
    VmaAllocator allocator_;
    vk::Image image_;
    VmaAllocation allocation_;
  };

  template<typename H>
  H AbslHashValue(H state, GpuImage const &image) noexcept {
    return H::combine(std::move(state), image.get());
  }
} // namespace imp