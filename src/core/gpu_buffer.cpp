#include "gpu_buffer.h"

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
            nullptr) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create vulkan buffer."};
    }
  }

  gpu_buffer::~gpu_buffer() {
    vmaDestroyBuffer(allocator_, buffer_, allocation_);
  }

  vk::Buffer gpu_buffer::get() const noexcept {
    return buffer_;
  }
} // namespace imp