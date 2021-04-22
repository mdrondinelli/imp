#pragma once

#include "../core/gpu_context.h"
#include "../core/worker_thread.h"
#include "../graphics/static_mesh_cache.h"
#include "../graphics/static_mesh_loader.h"

namespace imp {
  class client {
  public:
    client();
    ~client();

  private:
    gpu_context gpu_context_;
    worker_thread loading_thread_;
    static_mesh_loader static_mesh_loader_;
    static_mesh_cache static_mesh_cache_;
  };
} // namespace imp