#pragma once

#include <memory>

#include "../core/GpuContext.h"
#include "Atmosphere.h"
#include "Camera.h"
#include "DirectionalLight.h"

namespace imp {
  struct SceneCreateInfo {
    GpuContext *context = nullptr;
    std::shared_ptr<Atmosphere> atmosphere;
    std::shared_ptr<DirectionalLight> sunLight;
    std::shared_ptr<DirectionalLight> moonLight;
    std::shared_ptr<Camera> camera;
  };

  class Scene {
  public:
    explicit Scene(SceneCreateInfo const &createInfo) noexcept;
    explicit Scene(SceneCreateInfo &&createInfo) noexcept;

    std::shared_ptr<Atmosphere> getAtmosphere() const noexcept;
    std::shared_ptr<DirectionalLight> getSunLight() const noexcept;
    std::shared_ptr<DirectionalLight> getMoonLight() const noexcept;
    std::shared_ptr<Camera> getCamera() const noexcept;

  private:
    std::shared_ptr<Atmosphere> atmosphere_;
    std::shared_ptr<DirectionalLight> sunLight_;
    std::shared_ptr<DirectionalLight> moonLight_;
    std::shared_ptr<Camera> camera_;
  };
} // namespace imp