#pragma once

#include <vulkan/vulkan.hpp>

namespace imp {
  struct atmosphere_system_create_info {
    uint32_t graphics_family;
    uint32_t compute_family;
    vk::Device device;
  };
}