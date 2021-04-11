#pragma once

#include <vulkan/vulkan.hpp>

#include "../math/vector.h"

namespace imp {
  struct window_manager_create_info {
    vk::Instance instance;
    vector2u size;
    char const *title;
  };
} // namespace imp