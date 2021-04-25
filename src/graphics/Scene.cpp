#include "Scene.h"

namespace imp {
  Scene::Scene(SceneCreateInfo const &createInfo) noexcept:
      atmosphere_{createInfo.atmosphere}, camera_{createInfo.camera} {}

  Scene::Scene(SceneCreateInfo &&createInfo) noexcept:
      atmosphere_{std::move(createInfo.atmosphere)},
      camera_{std::move(createInfo.camera)} {}

  std::shared_ptr<Atmosphere> Scene::getAtmosphere() const noexcept {
    return atmosphere_;
  }

  std::shared_ptr<Camera> Scene::getCamera() const noexcept {
    return camera_;
  }
} // namespace imp