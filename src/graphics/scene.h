#pragma once

#include "Atmosphere.h"
#include "Camera.h"

namespace imp {
  struct SceneCreateInfo {
    GpuContext *context = nullptr;
    float planetRadius = 6360e3f;
    float atmosphereRadius = 6420e3f;
    Vector4f scatteringCoefficients =
        makeVector(3.8e-6f, 13.5e-6f, 33.1e-6f, 21e-6f);
    Vector2u opticalDepthLutSize = makeVector(512u, 512u);
    Vector3u scatteringLutSize = makeVector(64u, 256u, 256u);
    Transform cameraTransform;
    float tanHalfFovX = 1.0f;
    float tanHalfFovY = 1.0f;
    float minDepth = 0.25f;
    float maxDepth = 256.0f;
  };

  class Scene {
  public:
    explicit Scene(SceneCreateInfo const &createInfo);

    Atmosphere const &getAtmosphere() const noexcept;
    Atmosphere &getAtmosphere() noexcept;
    Camera const &getCamera() const noexcept;
    Camera &getCamera() noexcept;

  private:
    Atmosphere atmosphere_;
    Camera camera_;

    Atmosphere createAtmosphere(SceneCreateInfo const &createInfo);
    Camera createCamera(SceneCreateInfo const &createInfo);
  };
} // namespace imp