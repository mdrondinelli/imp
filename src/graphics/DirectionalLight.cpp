#include "DirectionalLight.h"

namespace imp {
  DirectionalLight::DirectionalLight(
      Spectrum const &irradiance, Eigen::Vector3f const &direction) noexcept:
      irradiance_{irradiance}, direction_{direction} {}

  Spectrum const &DirectionalLight::getIrradiance() const noexcept {
    return irradiance_;
  }

  Eigen::Vector3f const &DirectionalLight::getDirection() const noexcept {
    return direction_;
  }

  void DirectionalLight::setIrradiance(Spectrum const &irradiance) noexcept {
    irradiance_ = irradiance;
  }

  void
  DirectionalLight::setDirection(Eigen::Vector3f const &direction) noexcept {
    direction_ = direction;
  }
} // namespace imp