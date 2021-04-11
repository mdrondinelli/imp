#include "gpu_allocator.h"

#include <stdexcept>

namespace imp {
  gpu_allocator::gpu_allocator(gpu_allocator_create_info const &create_info):
      instance_{create_info.instance},
      physical_device_{create_info.physical_device},
      device_{create_info.device} {
    auto info = VmaAllocatorCreateInfo{};
    info.physicalDevice = physical_device_;
    info.device = device_;
    info.instance = instance_;
    info.vulkanApiVersion = VK_API_VERSION_1_1;
    if (vmaCreateAllocator(&info, &allocator_) != VK_SUCCESS) {
      throw std::runtime_error{"Failed to create vulkan allocator."};
    }
  }

  gpu_allocator::~gpu_allocator() {
    vmaDestroyAllocator(allocator_);
  }

  gpu_buffer gpu_allocator::create_buffer(
      vk::BufferCreateInfo const &buffer_info,
      VmaAllocationCreateInfo const &allocation_info) {
    return gpu_buffer{buffer_info, allocation_info, allocator_};
  }

  gpu_image gpu_allocator::create_image(
      vk::ImageCreateInfo const &image_info,
      VmaAllocationCreateInfo const &allocation_info) {
    return gpu_image{image_info, allocation_info, allocator_};
  }
} // namespace imp