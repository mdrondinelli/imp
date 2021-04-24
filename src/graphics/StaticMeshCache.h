#pragma once

#include <filesystem>

#include "../core/ResourceCache.h"
#include "StaticMesh.h"

namespace imp {
  using StaticMeshCache = ResourceCache<StaticMesh, std::filesystem::path>;
}