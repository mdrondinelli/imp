#include "Scene.h"

namespace imp {
  std::shared_ptr<Atmosphere> Scene::getAtmosphere() const noexcept {
    return atmosphere_;
  }

  std::shared_ptr<DirectionalLight> Scene::getSunLight() const noexcept {
    return sunLight_;
  }

  std::shared_ptr<DirectionalLight> Scene::getMoonLight() const noexcept {
    return moonLight_;
  }

  void Scene::setAtmosphere(std::shared_ptr<Atmosphere> atmosphere) noexcept {
    atmosphere_ = std::move(atmosphere);
  }
  
  void Scene::setSunLight(std::shared_ptr<DirectionalLight> light) noexcept {
    sunLight_ = std::move(light);
  }
  
  void Scene::setMoonLight(std::shared_ptr<DirectionalLight> light) noexcept {
    moonLight_ = std::move(light);
  }
} // namespace imp