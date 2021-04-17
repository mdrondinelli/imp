#pragma once

#include <entt/entt.hpp>

#include "../math/quaternion.h"

namespace imp {
  class scene {
  public:
    quaternionf const &orientation(entt::entity e) const;
    vector3f const &translation(entt::entity e) const;

    entt::entity create();
    void destroy(entt::entity e);

  private:
    entt::registry registry_;
  };
} // namespace imp