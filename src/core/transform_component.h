#pragma once

#include <vector>

#include <entt/entt.hpp>

#include "../math/Matrix.h"

namespace imp {
  struct transform_component {
    std::vector<entt::entity> children;
    entt::entity parent = entt::null;
    Vector3f translation = zeroVector<3, float>();
    Quaternionf rotation = identityQuaternion<float>();
    Matrix4x4f matrix = identityMatrix<4, 4, float>();
    bool matrix_valid = true;
  };
} // namespace imp