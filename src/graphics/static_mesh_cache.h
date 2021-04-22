#pragma once

#include <filesystem>

#include "../core/resource_cache.h"
#include "static_mesh.h"

namespace imp {
  using static_mesh_cache = resource_cache<static_mesh, std::filesystem::path>;
}