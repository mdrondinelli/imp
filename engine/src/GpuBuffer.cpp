// clang-format off
module;
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
module mobula.engine.gpu;
import <stdexcept>;
// clang-format on

namespace mobula {
  GpuBuffer::GpuBuffer(VmaAllocator allocator, GpuBufferParams const &params):
      allocator_{allocator}, params_{params} {
    auto bufferCreateInfo = vk::BufferCreateInfo{};
    bufferCreateInfo.flags = params.flags;
    bufferCreateInfo.size = params.size;
    bufferCreateInfo.usage = params.usage;
    if (!params.queueFamilyIndices.empty()) {
      bufferCreateInfo.sharingMode = vk::SharingMode::eConcurrent;
      bufferCreateInfo.queueFamilyIndexCount =
          static_cast<std::uint32_t>(params.queueFamilyIndices.size());
      bufferCreateInfo.pQueueFamilyIndices = params.queueFamilyIndices.data();
    }
    auto allocationCreateInfo = VmaAllocationCreateInfo{};
    allocationCreateInfo.flags = params.allocation.flags;
    allocationCreateInfo.usage = params.allocation.usage;
    if (auto result = vmaCreateBuffer(
            allocator_,
            reinterpret_cast<VkBufferCreateInfo *>(&bufferCreateInfo),
            &allocationCreateInfo,
            reinterpret_cast<VkBuffer *>(&buffer_),
            &allocation_,
            nullptr);
        result != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create GpuBuffer."};
    }
  }

  GpuBuffer::~GpuBuffer() {
    vmaDestroyBuffer(allocator_, buffer_, allocation_);
  }

  GpuBuffer::GpuBuffer(GpuBuffer &&rhs) noexcept:
      allocator_{rhs.allocator_},
      params_{rhs.params_},
      buffer_{rhs.buffer_},
      allocation_{rhs.allocation_} {
    rhs.buffer_ = nullptr;
    rhs.allocation_ = nullptr;
  }

  GpuBuffer &GpuBuffer::operator=(GpuBuffer &&rhs) noexcept {
    if (&rhs != this) {
      allocator_ = rhs.allocator_;
      params_ = rhs.params_;
      buffer_ = rhs.buffer_;
      allocation_ = rhs.allocation_;
      rhs.buffer_ = nullptr;
      rhs.allocation_ = nullptr;
    }
    return *this;
  }

  MappedMemory GpuBuffer::map() noexcept {
    return MappedMemory{allocator_, allocation_};
  }
} // namespace mobula