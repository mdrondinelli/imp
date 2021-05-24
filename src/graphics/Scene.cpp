#include "Scene.h"

namespace imp {
  Scene::Scene(SceneCreateInfo const &createInfo) noexcept:
      atmosphere_{createInfo.atmosphere},
      sunLight_{createInfo.sunLight},
      moonLight_{createInfo.moonLight},
      camera_{createInfo.camera} {}

  Scene::Scene(SceneCreateInfo &&createInfo) noexcept:
      atmosphere_{std::move(createInfo.atmosphere)},
      sunLight_{std::move(createInfo.sunLight)},
      moonLight_{std::move(createInfo.moonLight)},
      camera_{std::move(createInfo.camera)} {}

  std::shared_ptr<Atmosphere> Scene::getAtmosphere() const noexcept {
    return atmosphere_;
  }

  std::shared_ptr<DirectionalLight> Scene::getSunLight() const noexcept {
    return sunLight_;
  }

  std::shared_ptr<DirectionalLight> Scene::getMoonLight() const noexcept {
    return moonLight_;
  }

  std::shared_ptr<Camera> Scene::getCamera() const noexcept {
    return camera_;
  }
} // namespace imp