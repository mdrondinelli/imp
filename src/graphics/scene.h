#pragma once

#include <memory>

#include "Atmosphere.h"
#include "Camera.h"

namespace imp {
  struct SceneCreateInfo {
    GpuContext *context = nullptr;
    std::shared_ptr<Atmosphere> atmosphere;
    std::shared_ptr<Camera> camera;
  };

  class Scene {
  public:
    explicit Scene(SceneCreateInfo const &createInfo) noexcept;
    explicit Scene(SceneCreateInfo &&createInfo) noexcept;

    std::shared_ptr<Atmosphere> getAtmosphere() const noexcept;
    std::shared_ptr<Camera> getCamera() const noexcept;

  private:
    std::shared_ptr<Atmosphere> atmosphere_;
    std::shared_ptr<Camera> camera_;
  };
} // namespace imp