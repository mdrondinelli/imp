#pragma once

#include "../graphics/static_mesh_cache.h"
#include "../graphics/static_mesh_loader.h"
#include "gpu_context.h"
#include "resource_cache.h"

namespace imp {
  class client {
  public:
  private:
    gpu_context context_;
    static_mesh_loader static_mesh_loader_;
    worker_thread loader_thread_;
    static_mesh_cache static_mesh_cache_;
  };
} // namespace imp