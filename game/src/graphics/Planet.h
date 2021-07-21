#pragma once

#include "Spectrum.h"

namespace imp {
  class Planet {
  public:
    static constexpr auto UNIFORM_SIZE = std::size_t{100};
    static constexpr auto UNIFORM_ALIGN = std::size_t{16};

    explicit Planet(
        Eigen::Vector3f const &position = {0.0f, 0.0f, 0.0f},
        float groundRadius = 6360e3f,
        float atmosphereRadius = 6460e3f,
        Spectrum const &albedo = {0.3f, 0.3f, 0.3f},
        Spectrum const &rayleighScattering = {5.802e-6f, 13.558e-6f, 33.1e-6f},
        float rayleighScaleHeight = 8000.0f,
        float mieScattering = 3.996e-6f,
        float mieAbsorption = 4.40e-6f,
        float mieScaleHeight = 1200.0f,
        float mieG = 0.8f,
        Spectrum const &ozoneAbsorption = {0.650e-6f, 1.881e-6f, 0.085e-6f},
        float ozoneLayerHeight = 25000.0f,
        float ozoneLayerThickness = 30000.0f) noexcept;

    void store(char *dst) const noexcept;

    Eigen::Vector3f const &getPosition() const noexcept;
    void setPosition(Eigen::Vector3f const &position) noexcept;

    float getGroundRadius() const noexcept;
    void setGroundRadius(float groundRadius) noexcept;

    float getAtmosphereRadius() const noexcept;
    void setAtmosphereRadius(float atmosphereRadius) noexcept;

    Spectrum const &getAlbedo() const noexcept;
    void setAlbedo(Spectrum const &albedo) noexcept;

    Spectrum const &getRayleighScattering() const noexcept;
    void setRayleighScattering(Spectrum const &scattering) noexcept;

    float getRayleighScaleHeight() const noexcept;
    void setRayleighScaleHeight(float scaleHeight) noexcept;

    float getMieScattering() const noexcept;
    void setMieScattering(float scattering) noexcept;

    float getMieAbsorption() const noexcept;
    void setMieAbsorption(float absorption) noexcept;

    float getMieG() const noexcept;
    void setMieG(float g) noexcept;

    float getMieScaleHeight() const noexcept;
    void setMieScaleHeight(float scaleHeight) noexcept;

    Spectrum const &getOzoneAbsorption() const noexcept;
    void setOzoneAbsorption(Spectrum const &absorption) noexcept;

    float getOzoneLayerHeight() const noexcept;
    void setOzoneLayerHeight(float height) noexcept;

    float getOzoneLayerThickness() const noexcept;
    void setOzoneLayerThickness(float thickness) noexcept;

  private:
    Eigen::Vector3f position_;
    float groundRadius_;
    float atmosphereRadius_;
    Spectrum albedo_;
    Spectrum rayleighScattering_;
    float rayleighScaleHeight_;
    float mieScattering_;
    float mieAbsorption_;
    float mieScaleHeight_;
    float mieG_;
    Spectrum ozoneAbsorption_;
    float ozoneLayerHeight_;
    float ozoneLayerThickness_;
  };
} // namespace imp