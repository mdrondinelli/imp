#pragma once

#include "../core/gpu_context.h"
#include "../core/window.h"
#include "../core/worker_thread.h"
#include "../graphics/renderer.h"
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
    window window_;
    renderer renderer_;
  };
} // namespace imp