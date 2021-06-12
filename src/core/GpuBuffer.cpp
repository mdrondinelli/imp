#include "GpuBuffer.h"

#include <stdexcept>

namespace imp {
  GpuBuffer::GpuBuffer(
      VmaAllocator allocator,
      vk::BufferCreateInfo const &buffer,
      VmaAllocationCreateInfo const &allocation):
      allocator_{allocator} {
    if (vmaCreateBuffer(
            allocator_,
            reinterpret_cast<VkBufferCreateInfo const *>(&buffer),
            &allocation,
            reinterpret_cast<VkBuffer *>(&buffer_),
            &allocation_,
            nullptr) != VK_SUCCESS) {
      throw std::runtime_error{"failed to create gpu buffer."};
    }
  }

  GpuBuffer::~GpuBuffer() {
    if (allocator_) {
      vmaDestroyBuffer(allocator_, buffer_, allocation_);
    }
  }

  GpuBuffer::GpuBuffer(GpuBuffer &&rhs) noexcept:
      allocator_{rhs.allocator_},
      buffer_{rhs.buffer_},
      allocation_{rhs.allocation_} {
    rhs.allocator_ = nullptr;
  }

  GpuBuffer &GpuBuffer::operator=(GpuBuffer &&rhs) noexcept {
    if (&rhs != this) {
      if (allocator_) {
        vmaDestroyBuffer(allocator_, buffer_, allocation_);
      }
      allocator_ = rhs.allocator_;
      buffer_ = rhs.buffer_;
      allocation_ = rhs.allocation_;
      rhs.allocator_ = nullptr;
    }
    return *this;
  }

  vk::Buffer GpuBuffer::get() const noexcept {
    return buffer_;
  }

  char *GpuBuffer::data() const noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    return static_cast<char *>(info.pMappedData);
  }

  char *GpuBuffer::map() {
    void *data;
    if (vmaMapMemory(allocator_, allocation_, &data)) {
      throw std::runtime_error{"failed to map gpu buffer memory."};
    }
    return static_cast<char *>(data);
  }

  void GpuBuffer::unmap() noexcept {
    vmaUnmapMemory(allocator_, allocation_);
  }

  void GpuBuffer::flush() noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    vmaFlushAllocation(allocator_, allocation_, info.offset, info.size);
  }

  void GpuBuffer::invalidate() noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    vmaInvalidateAllocation(allocator_, allocation_, info.offset, info.size);
  }
} // namespace imp