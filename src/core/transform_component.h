#pragma once

#include <vector>

#include <entt/entt.hpp>

#include "../math/matrix.h"

namespace imp {
  struct transform_component {
    std::vector<entt::entity> children;
    entt::entity parent = entt::null;
    vector3f translation = zero_vector<3, float>();
    quaternionf rotation = identity_quaternion<float>();
    matrix4x4f matrix = identity_matrix<4, 4, float>();
    bool matrix_valid = true;
  };
} // namespace imp