#pragma once

#include <vulkan/vulkan.hpp>

#include "../util/Gsl.h"
#include "vk_mem_alloc.h"

namespace imp {
  class GpuBuffer {
  public:
    explicit GpuBuffer(
        gsl::not_null<VmaAllocator> allocator,
        vk::BufferCreateInfo const &bufferCreateInfo,
        VmaAllocationCreateInfo const &allocationCreateInfo,
        std::string_view name = "GpuBuffer");
    ~GpuBuffer();

    GpuBuffer(GpuBuffer &&rhs) noexcept;
    GpuBuffer &operator=(GpuBuffer &&rhs) noexcept;

    gsl::not_null<char *> map();
    void unmap() noexcept;
    void flush() noexcept;
    void flush(vk::DeviceSize offset, vk::DeviceSize size) noexcept;
    void invalidate() noexcept;
    void invalidate(vk::DeviceSize offset, vk::DeviceSize size) noexcept;

    char *getMappedData() const noexcept;
    vk::BufferCreateFlags getFlags() const noexcept;
    vk::DeviceSize getSize() const noexcept;
    vk::BufferUsageFlags getUsage() const noexcept;
    vk::SharingMode getSharingMode() const noexcept;
    vk::Buffer get() const noexcept;

  private:
    gsl::not_null<VmaAllocator> allocator_;
    vk::BufferCreateFlags flags_;
    vk::DeviceSize size_;
    vk::BufferUsageFlags usage_;
    vk::SharingMode sharingMode_;
    vk::Buffer buffer_;
    VmaAllocation allocation_;
  };
} // namespace imp