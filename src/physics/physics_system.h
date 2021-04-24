#pragma once

#include <bullet/btBulletDynamicsCommon.h>

namespace imp {
  class physics_system {
  public:
    physics_system(float timestep = 1.0f / 60.0f);
    void tick(float dt);

  private:
    btDefaultCollisionConfiguration config_;
    btCollisionDispatcher dispatcher_;
    btDbvtBroadphase broadphase_;
    btSequentialImpulseConstraintSolver solver_;
    btDiscreteDynamicsWorld world_;
    float timestep_;
  };
} // namespace imp