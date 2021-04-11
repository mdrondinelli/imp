#include "world.h"

#include <chrono>

namespace imp {
  world::world(): transform_system_{&registry_} {}

  void world::tick(float dt) {
    physics_system_.tick(dt);
  }

  entt::registry const &world::get_registry() const noexcept {
    return registry_;
  }

  entt::registry &world::get_registry() noexcept {
    return registry_;
  }
} // namespace imp