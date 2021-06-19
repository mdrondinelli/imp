#pragma once

#include "Spectrum.h"

namespace imp {
  class Atmosphere {
  public:
    explicit Atmosphere(
        Spectrum const &rayleighScattering =
            {5.802e-6f, 13.558e-6f, 33.1e-6f},
        float rayleighScaleHeight = 8000.0f,
        float mieScattering = 3.996e-6f,
        float mieAbsorption = 4.40e-6f,
        float mieScaleHeight = 1200.0f,
        float mieG = 0.8f,
        Spectrum const &ozoneAbsorption =
            {0.650e-6f, 1.881e-6f, 0.085e-6f},
        float ozoneLayerHeight = 25000.0f,
        float ozoneLayerThickness = 30000.0f,
        float planetRadius = 6360e3f,
        float atmosphereRadius = 6460e3f) noexcept;

    Spectrum const &getRayleighScattering() const noexcept;
    float getRayleighScaleHeight() const noexcept;
    float getMieScattering() const noexcept;
    float getMieAbsorption() const noexcept;
    float getMieG() const noexcept;
    float getMieScaleHeight() const noexcept;
    Spectrum const &getOzoneAbsorption() const noexcept;
    float getOzoneLayerHeight() const noexcept;
    float getOzoneLayerThickness() const noexcept;
    float getPlanetRadius() const noexcept;
    float getAtmosphereRadius() const noexcept;

    void setRayleighScattering(Spectrum const &scattering) noexcept;
    void setRayleighScaleHeight(float scaleHeight) noexcept;
    void setMieScattering(float scattering) noexcept;
    void setMieAbsorption(float absorption) noexcept;
    void setMieScaleHeight(float scaleHeight) noexcept;
    void setMieG(float g) noexcept;
    void setOzoneAbsorption(Spectrum const &absorption) noexcept;
    void setOzoneLayerHeight(float height) noexcept;
    void setOzoneLayerThickness(float thickness) noexcept;
    void setPlanetRadius(float planetRadius) noexcept;
    void setAtmosphereRadius(float atmosphereRadius) noexcept;

  private:
    Spectrum rayleighScattering_;
    float rayleighScaleHeight_;
    float mieScattering_;
    float mieAbsorption_;
    float mieScaleHeight_;
    float mieG_;
    Spectrum ozoneAbsorption_;
    float ozoneLayerHeight_;
    float ozoneLayerThickness_;
    float planetRadius_;
    float atmosphereRadius_;
  };
} // namespace imp