#pragma once

#include <entt/entt.hpp>

namespace imp {
  class Composition {
  public:
    entt::entity createElement();


  private:
    entt::registry registry_;
  };
} // namespace imp