#pragma once

#include <vulkan/vulkan.hpp>

namespace imp {
  struct gpu_allocator_create_info {
    vk::Instance instance;
    vk::PhysicalDevice physical_device;
    vk::Device device;
  };
} // namespace imp