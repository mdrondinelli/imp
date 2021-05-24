#pragma once

#include "../math/Vector.h"

namespace imp {
  struct DirectionalLightCreateInfo {
    Vector3f irradiance = {1.0f, 1.0f, 1.0f};
    Vector3f direction = {0.0f, 1.0f, 0.0f};
  };

  class DirectionalLight {
  public:
    explicit DirectionalLight(
        DirectionalLightCreateInfo const &createInfo) noexcept;

    Vector3f const &getIrradiance() const noexcept;
    Vector3f const &getDirection() const noexcept;

    void setIrradiance(Vector3f const &irradiance) noexcept;
    void setDirection(Vector3f const &direction) noexcept;

  private:
    Vector3f irradiance_;
    Vector3f direction_;
  };
} // namespace imp