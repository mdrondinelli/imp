#include "DirectionalLight.h"

namespace imp {
  DirectionalLight::DirectionalLight(
      Spectrum const &irradiance,
      Eigen::Vector3f const &direction,
      float cosAngularRadius) noexcept:
      irradiance_{irradiance},
      direction_{direction},
      cosAngularRadius_{cosAngularRadius} {}

  void DirectionalLight::store(char* dst) const noexcept {
    std::memcpy(dst + 0, irradiance_.data(), 12);
    std::memcpy(dst + 16, direction_.data(), 12);
    std::memcpy(dst + 28, &cosAngularRadius_, 4);
  }

  Spectrum const &DirectionalLight::getIrradiance() const noexcept {
    return irradiance_;
  }

  void DirectionalLight::setIrradiance(Spectrum const &irradiance) noexcept {
    irradiance_ = irradiance;
  }

  Eigen::Vector3f const &DirectionalLight::getDirection() const noexcept {
    return direction_;
  }

  void
  DirectionalLight::setDirection(Eigen::Vector3f const &direction) noexcept {
    direction_ = direction;
  }

  float DirectionalLight::getCosAngularRadius() const noexcept {
    return cosAngularRadius_;
  }

  void DirectionalLight::setCosAngularRadius(float cosAngularRadius) noexcept {
    cosAngularRadius_ = cosAngularRadius;
  }
} // namespace imp