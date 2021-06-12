#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.h"

namespace imp {
  class GpuContext;
  class GpuBuffer {
  public:
    GpuBuffer(
        VmaAllocator allocator,
        vk::BufferCreateInfo const &buffer,
        VmaAllocationCreateInfo const &allocation);
    ~GpuBuffer();

    GpuBuffer(GpuBuffer &&rhs) noexcept;
    GpuBuffer &operator=(GpuBuffer &&rhs) noexcept;

    vk::Buffer get() const noexcept;
    char *data() const noexcept;
    
    char *map();
    void unmap() noexcept;
    void flush() noexcept;
    void invalidate() noexcept;

  private:
    VmaAllocator allocator_;
    vk::Buffer buffer_;
    VmaAllocation allocation_;
  };
} // namespace imp