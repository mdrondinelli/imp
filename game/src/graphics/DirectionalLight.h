#pragma once

#include "Spectrum.h"

namespace imp {
  class DirectionalLight {
  public:
    static constexpr auto UNIFORM_SIZE = std::size_t{32};
    static constexpr auto UNIFORM_ALIGN = std::size_t{16};

    explicit DirectionalLight(
        Spectrum const &irradiance,
        Eigen::Vector3f const &direction,
        float cosAngularRadius = 0.999989f) noexcept;

    void store(char *dst) const noexcept;

    Spectrum const &getIrradiance() const noexcept;
    void setIrradiance(Spectrum const &irradiance) noexcept;

    Eigen::Vector3f const &getDirection() const noexcept;
    void setDirection(Eigen::Vector3f const &direction) noexcept;

    float getCosAngularRadius() const noexcept;
    void setCosAngularRadius(float cosAngularRadius) noexcept;

  private:
    Spectrum irradiance_;
    Eigen::Vector3f direction_;
    float cosAngularRadius_;
  };
} // namespace imp