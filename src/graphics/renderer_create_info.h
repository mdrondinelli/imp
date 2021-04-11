#pragma once

#include "../core/gpu_manager.h"
#include "../core/window.h"

namespace imp {
  struct renderer_create_info {
    gpu_manager *gpu_manager;
    window_manager *window_manager;
  };
} // namespace imp