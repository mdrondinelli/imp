#pragma once

#include <memory>
#include <unordered_set>

#include <entt/entt.hpp>

#include "../physics/physics_system.h"
#include "transform_system.h"

namespace imp {
  /**
   * @brief A world.
   */
  class world {
  public:
    /**
     * @brief Constructs an empty world.
     */
    world();

    world(world const &rhs) = delete;
    world &operator=(world const &rhs) = delete;

    /**
     * @brief Ticks the world.
     */
    void tick(float dt);

    /**
     * @returns the registry.
     */
    entt::registry const &get_registry() const noexcept;

    /**
     * @returns the registry.
     */
    entt::registry &get_registry() noexcept;

  private:
    entt::registry registry_;
    transform_system transform_system_;
    physics_system physics_system_;
  };
} // namespace imp