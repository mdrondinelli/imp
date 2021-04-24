#include "Scene.h"

namespace imp {
  Scene::Scene(SceneCreateInfo const &createInfo):
      atmosphere_{createAtmosphere(createInfo)},
      camera_{createCamera(createInfo)} {}

  Atmosphere const &Scene::getAtmosphere() const noexcept {
    return atmosphere_;
  }

  Atmosphere &Scene::getAtmosphere() noexcept {
    return atmosphere_;
  }

  Camera const &Scene::getCamera() const noexcept {
    return camera_;
  }

  Camera &Scene::getCamera() noexcept {
    return camera_;
  }

  Atmosphere Scene::createAtmosphere(SceneCreateInfo const &createInfo) {
    auto atmosphereCreateInfo = AtmosphereCreateInfo{};
    atmosphereCreateInfo.context = createInfo.context;
    atmosphereCreateInfo.planetRadius = createInfo.planetRadius;
    atmosphereCreateInfo.atmosphereRadius = createInfo.atmosphereRadius;
    atmosphereCreateInfo.scatteringCoefficients =
        createInfo.scatteringCoefficients;
    atmosphereCreateInfo.opticalDepthLutSize = createInfo.opticalDepthLutSize;
    atmosphereCreateInfo.scatteringLutSize = createInfo.scatteringLutSize;
    return Atmosphere{atmosphereCreateInfo};
  }

  Camera Scene::createCamera(SceneCreateInfo const &createInfo) {
    auto cameraCreateInfo = CameraCreateInfo{};
    cameraCreateInfo.transform = createInfo.cameraTransform;
    cameraCreateInfo.tanHalfFovX = createInfo.tanHalfFovX;
    cameraCreateInfo.tanHalfFovY = createInfo.tanHalfFovY;
    cameraCreateInfo.minDepth = createInfo.minDepth;
    cameraCreateInfo.maxDepth = createInfo.maxDepth;
    return Camera{cameraCreateInfo};
  }
} // namespace imp