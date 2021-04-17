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

    gpu_buffer(gpu_buffer &&rhs) noexcept;
    gpu_buffer &operator=(gpu_buffer &&rhs) noexcept;

    vk::Buffer const *operator->() const noexcept {
      return &buffer_;
    }

    vk::Buffer const &operator*() const noexcept {
      return buffer_;
    }

    operator bool() const noexcept {
      return buffer_;
    }

    vk::DeviceMemory device_memory() const noexcept {
      return allocation_info_.deviceMemory;
    }

    vk::DeviceSize offset() const noexcept {
      return allocation_info_.offset;
    }

    vk::DeviceSize size() const noexcept {
      return allocation_info_.size;
    }

    char const *data() const noexcept {
      return static_cast<char *>(allocation_info_.pMappedData);
    }

    char *data() noexcept {
      return static_cast<char *>(allocation_info_.pMappedData);
    }

    void map();
    void unmap() noexcept;

  private:
    vk::Buffer buffer_;
    VmaAllocation allocation_;
    VmaAllocationInfo allocation_info_;
    VmaAllocator allocator_;
  };
} // namespace imp