#pragma once

#include <vulkan/vulkan.hpp>

#include "../math/vector.h"

namespace imp {
  struct scattering_lut_create_info {
    vk::Device device;
    vector3u size;
    uint32_t graphics_family;
    uint32_t compute_family;
  };
}