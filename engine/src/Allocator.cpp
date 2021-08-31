// clang-format off
module;
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
module mobula.gpu;
import <stdexcept>;
// clang-format on

namespace mobula {
  namespace gpu {
    Allocator::Allocator(
        vk::PhysicalDevice physicalDevice,
        vk::Device device,
        vk::Instance instance) {
      auto createInfo = VmaAllocatorCreateInfo{};
      createInfo.physicalDevice = physicalDevice;
      createInfo.device = device;
      createInfo.instance = instance;
      createInfo.vulkanApiVersion = VK_API_VERSION_1_2;
      if (vmaCreateAllocator(&createInfo, &allocator_) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to construct gpu::Allocator."};
      }
    }

    Allocator::Allocator(Allocator &&rhs) noexcept: allocator_{rhs.allocator_} {
      rhs.allocator_ = nullptr;
    }

    Allocator& Allocator::operator=(Allocator&& rhs) noexcept {
      if (&rhs != this) {
        if (allocator_) {
          vmaDestroyAllocator(allocator_);
        }
        allocator_ = rhs.allocator_;
        rhs.allocator_ = nullptr;
      }
      return *this;
    }

    Allocator::~Allocator() {
      if (allocator_) {
        vmaDestroyAllocator(allocator_);
      }
    }

    Buffer Allocator::create(
      BufferParams const& bufferParams,
      AllocationParams const& allocationParams) {
      return Buffer{allocator_, bufferParams, allocationParams};
    }

    Image Allocator::create(
      ImageParams const& imageParams,
      AllocationParams const& allocationParams) {
      return Image{allocator_, imageParams, allocationParams};
    }
  } // namespace gpu
} // namespace mobula