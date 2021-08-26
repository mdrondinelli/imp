// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
module mobula.engine.vulkan;
import <stdexcept>;
// clang-format on

namespace mobula {
  Buffer::Buffer(
      VmaAllocator allocator,
      BufferParams const &bufferParams,
      AllocationParams const &allocationParams):
      allocator_{allocator},
      bufferParams_{bufferParams_},
      allocationParams_{allocationParams} {
    auto bufferCreateInfo = vk::BufferCreateInfo{};
    bufferCreateInfo.size = bufferParams.size;
    bufferCreateInfo.usage = bufferParams.usage;
    if (!bufferParams.queueFamilyIndices.empty()) {
      bufferCreateInfo.sharingMode = vk::SharingMode::eConcurrent;
      bufferCreateInfo.queueFamilyIndexCount =
          static_cast<std::uint32_t>(bufferParams.queueFamilyIndices.size());
      bufferCreateInfo.pQueueFamilyIndices =
          bufferParams.queueFamilyIndices.data();
    }
    auto allocationCreateInfo = VmaAllocationCreateInfo{};
    allocationCreateInfo.flags = allocationParams.flags;
    allocationCreateInfo.usage = allocationParams.usage;
    if (vmaCreateBuffer(
            allocator,
            reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
            &allocationCreateInfo,
            reinterpret_cast<VkBuffer *>(&buffer_),
            &allocation_,
            nullptr) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create buffer."};
    }
  }

  Buffer::Buffer(Buffer &&rhs) noexcept:
      allocator_{rhs.allocator_},
      bufferParams_{std::move(rhs.bufferParams_)},
      allocationParams_{rhs.allocationParams_},
      buffer_{rhs.buffer_},
      allocation_{rhs.allocation_} {
    rhs.buffer_ = vk::Buffer{};
    rhs.allocation_ = nullptr;
  }

  Buffer &Buffer::operator=(Buffer &&rhs) noexcept {
    if (&rhs != this) {
      vmaDestroyBuffer(allocator_, buffer_, allocation_);
      allocator_ = rhs.allocator_;
      bufferParams_ = std::move(rhs.bufferParams_);
      allocationParams_ = rhs.allocationParams_;
      buffer_ = rhs.buffer_;
      allocation_ = rhs.allocation_;
      rhs.buffer_ = vk::Buffer{};
      rhs.allocation_ = nullptr;
    }
    return *this;
  }

  Buffer::~Buffer() {
    vmaDestroyBuffer(allocator_, buffer_, allocation_);
  }
} // namespace mobula