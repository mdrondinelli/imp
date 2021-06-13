#pragma once

#include "Spectrum.h"

namespace imp {
  class DirectionalLight {
  public:
    DirectionalLight(
        Spectrum const &irradiance,
        Eigen::Vector3f const &direction) noexcept;

    Spectrum const &getIrradiance() const noexcept;
    Eigen::Vector3f const &getDirection() const noexcept;

    void setIrradiance(Spectrum const &irradiance) noexcept;
    void setDirection(Eigen::Vector3f const &direction) noexcept;

  private:
    Spectrum irradiance_;
    Eigen::Vector3f direction_;
  };
} // namespace imp