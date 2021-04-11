#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.h"

namespace imp {
  class gpu_buffer {
  public:
    gpu_buffer(
        vk::BufferCreateInfo const &buffer_info,
        VmaAllocationCreateInfo const &allocation_info,
        VmaAllocator allocator);
    ~gpu_buffer();

    vk::Buffer get() const noexcept;

  private:
    vk::Buffer buffer_;
    VmaAllocation allocation_;
    VmaAllocator allocator_;
  };
} // namespace imp