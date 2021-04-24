#pragma once

#include "../core/GpuBuffer.h"
#include "../core/GpuContext.h"
#include "StaticVertex.h"

namespace imp {
  class StaticMesh {
  public:
  private:
    GpuBuffer vertexBuffer_;
    GpuBuffer indexBuffer_;
  };
} // namespace imp