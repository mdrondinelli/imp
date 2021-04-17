#include "gpu_buffer.h"

#include <stdexcept>

namespace imp {
  gpu_buffer::gpu_buffer(
      vk::BufferCreateInfo const &buffer_info,
      VmaAllocationCreateInfo const &allocation_info,
      VmaAllocator allocator):
      allocator_{allocator} {
    if (vmaCreateBuffer(
            allocator_,
            reinterpret_cast<VkBufferCreateInfo const *>(&buffer_info),
            &allocation_info,
            reinterpret_cast<VkBuffer *>(&buffer_),
            &allocation_,
            &allocation_info_) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create vulkan buffer."};
    }
  }

  gpu_buffer::~gpu_buffer() {
    if (buffer_) {
      vmaDestroyBuffer(allocator_, buffer_, allocation_);
    }
  }

  gpu_buffer::gpu_buffer(gpu_buffer &&rhs) noexcept:
      buffer_{rhs.buffer_},
      allocation_{rhs.allocation_},
      allocation_info_{rhs.allocation_info_},
      allocator_{rhs.allocator_} {
    rhs.buffer_ = nullptr;
  }

  gpu_buffer &gpu_buffer::operator=(gpu_buffer &&rhs) noexcept {
    if (&rhs != this) {
      if (buffer_) {
        vmaDestroyBuffer(allocator_, buffer_, allocation_);
      }
      buffer_ = rhs.buffer_;
      allocation_ = rhs.allocation_;
      allocation_info_ = rhs.allocation_info_;
      allocator_ = rhs.allocator_;
      rhs.buffer_ = nullptr;
    }
    return *this;
  }

  void gpu_buffer::map() {
    if (vmaMapMemory(allocator_, allocation_, &allocation_info_.pMappedData)) {
      throw std::runtime_error{"failed to map gpu buffer memory."};
    }
  }

  void gpu_buffer::unmap() noexcept {
    vmaUnmapMemory(allocator_, allocation_);
  }
} // namespace imp