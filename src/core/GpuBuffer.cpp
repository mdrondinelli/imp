#include "GpuBuffer.h"

#include <stdexcept>

namespace imp {
  GpuBuffer::GpuBuffer(
      vk::BufferCreateInfo const &bufferCreateInfo,
      VmaAllocationCreateInfo const &allocationCreateInfo,
      VmaAllocator allocator):
      allocator_{allocator} {
    if (vmaCreateBuffer(
            allocator_,
            reinterpret_cast<VkBufferCreateInfo const *>(&bufferCreateInfo),
            &allocationCreateInfo,
            reinterpret_cast<VkBuffer *>(&buffer_),
            &allocation_,
            &allocationInfo_) != VK_SUCCESS) {
      throw std::runtime_error{"failed to create gpu buffer."};
    }
  }

  GpuBuffer::~GpuBuffer() {
    if (buffer_) {
      vmaDestroyBuffer(allocator_, buffer_, allocation_);
    }
  }

  GpuBuffer::GpuBuffer(GpuBuffer &&rhs) noexcept:
      buffer_{rhs.buffer_},
      allocation_{rhs.allocation_},
      allocationInfo_{rhs.allocationInfo_},
      allocator_{rhs.allocator_} {
    rhs.buffer_ = nullptr;
  }

  GpuBuffer &GpuBuffer::operator=(GpuBuffer &&rhs) noexcept {
    if (&rhs != this) {
      if (buffer_) {
        vmaDestroyBuffer(allocator_, buffer_, allocation_);
      }
      buffer_ = rhs.buffer_;
      allocation_ = rhs.allocation_;
      allocationInfo_ = rhs.allocationInfo_;
      allocator_ = rhs.allocator_;
      rhs.buffer_ = nullptr;
    }
    return *this;
  }

  void GpuBuffer::reset() noexcept {
    if (buffer_) {
      vmaDestroyBuffer(allocator_, buffer_, allocation_);
      buffer_ = nullptr;
    }
  }

  void GpuBuffer::map() {
    if (vmaMapMemory(allocator_, allocation_, &allocationInfo_.pMappedData)) {
      throw std::runtime_error{"failed to map gpu buffer memory."};
    }
  }

  void GpuBuffer::unmap() noexcept {
    vmaUnmapMemory(allocator_, allocation_);
  }
} // namespace imp