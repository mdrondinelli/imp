#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.h"

namespace imp {
  class GpuBuffer {
  public:
    explicit GpuBuffer(
        vk::BufferCreateInfo const &bufferCreateInfo,
        VmaAllocationCreateInfo const &allocationCreateInfo,
        VmaAllocator allocator);
    ~GpuBuffer();

    GpuBuffer(GpuBuffer &&rhs) noexcept;
    GpuBuffer &operator=(GpuBuffer &&rhs) noexcept;

    void reset() noexcept;
    void map();
    void unmap() noexcept;

    vk::Buffer const *operator->() const noexcept {
      return &buffer_;
    }

    vk::Buffer const &operator*() const noexcept {
      return buffer_;
    }

    operator bool() const noexcept {
      return buffer_;
    }

    vk::DeviceMemory deviceMemory() const noexcept {
      return allocationInfo_.deviceMemory;
    }

    vk::DeviceSize offset() const noexcept {
      return allocationInfo_.offset;
    }

    vk::DeviceSize size() const noexcept {
      return allocationInfo_.size;
    }

    char const *data() const noexcept {
      return static_cast<char *>(allocationInfo_.pMappedData);
    }

    char *data() noexcept {
      return static_cast<char *>(allocationInfo_.pMappedData);
    }

  private:
    vk::Buffer buffer_;
    VmaAllocation allocation_;
    VmaAllocationInfo allocationInfo_;
    VmaAllocator allocator_;
  };
} // namespace imp