#pragma once

#include <filesystem>

#include "../core/ResourceLoader.h"
#include "StaticMesh.h"

namespace imp {
  class StaticMeshLoader:
      public ResourceLoader<StaticMesh, std::filesystem::path> {
  public:
    std::optional<StaticMesh> load(std::filesystem::path const &path) override;

  private:
    GpuContext *context_;
  };
} // namespace imp