#pragma once

#include <filesystem>

#include "../core/resource_loader.h"
#include "static_mesh.h"

namespace imp {
  class static_mesh_loader:
      public resource_loader<static_mesh, std::filesystem::path> {
  public:
    std::optional<static_mesh> load(std::filesystem::path const &path) override;

  private:
    gpu_context *context_;
  };
} // namespace imp