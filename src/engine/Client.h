#pragma once

#include "../core/Display.h"
#include "../core/GpuContext.h"
#include "../core/WorkerThread.h"
#include "../graphics/Renderer.h"
#include "../graphics/StaticMeshCache.h"
#include "../graphics/StaticMeshLoader.h"

namespace imp {
  class Client {
  public:
    Client();
    ~Client();

  private:
    GpuContext gpuContext_;
    WorkerThread loadingThread_;
    StaticMeshLoader staticMeshLoader_;
    StaticMeshCache staticMeshCache_;
    Display display_;
    Renderer renderer_;
  };
} // namespace imp