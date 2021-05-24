#include "DirectionalLight.h"

namespace imp {
  DirectionalLight::DirectionalLight(
      DirectionalLightCreateInfo const &createInfo) noexcept:
      irradiance_{createInfo.irradiance}, direction_{createInfo.direction} {}

  Vector3f const &DirectionalLight::getIrradiance() const noexcept {
    return irradiance_;
  }

  Vector3f const &DirectionalLight::getDirection() const noexcept {
    return direction_;
  }

  void DirectionalLight::setIrradiance(Vector3f const &irradiance) noexcept {
    irradiance_ = irradiance;
  }

  void DirectionalLight::setDirection(Vector3f const &direction) noexcept {
    direction_ = direction;
  }
} // namespace imp