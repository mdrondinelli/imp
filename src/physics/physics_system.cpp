#include "physics_system.h"

namespace imp {
  physics_system::physics_system(float timestep):
      dispatcher_{&config_},
      world_{&dispatcher_, &broadphase_, &solver_, &config_},
      timestep_{timestep} {}

  void physics_system::tick(float dt) {
    world_.stepSimulation(dt, 1, timestep_);
  }
} // namespace imp