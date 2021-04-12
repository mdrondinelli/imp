#pragma once

#include <vulkan/vulkan.hpp>

#include "../math/vector.h"

namespace imp {
  struct optical_depth_lut_create_info {
    vk::Device device;
    uint32_t graphics_family;
    uint32_t compute_family;
    vector2u size;
  };
}