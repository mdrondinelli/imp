#pragma once

#include "../math/Vector.h"

namespace imp {
  struct AtmosphereCreateInfo {
    float planetRadius = 6360e3f;
    float atmosphereRadius = 6460e3f;
    Vector3f rayleighScattering = {5.802e-6f, 13.558e-6f, 33.1e-6f};
    float rayleighScaleHeight = 8000.0f;
    float mieScattering = 3.996e-6f;
    float mieAbsorption = 4.40e-6f;
    float mieScaleHeight = 1200.0f;
    float mieG = 0.8f;
    Vector3f ozoneAbsorption = {0.650e-6f, 1.881e-6f, 0.085e-6f};
    float ozoneHeightCenter = 25000.0f;
    float ozoneHeightRange = 30000.0f;
  };

  class Atmosphere {
  public:
    explicit Atmosphere(AtmosphereCreateInfo const &createInfo) noexcept;

    float getPlanetRadius() const noexcept;
    float getAtmosphereRadius() const noexcept;
    Vector3f const &getRayleighScattering() const noexcept;
    float getRayleighScaleHeight() const noexcept;
    float getMieScattering() const noexcept;
    float getMieAbsorption() const noexcept;
    float getMieG() const noexcept;
    float getMieScaleHeight() const noexcept;
    Vector3f const &getOzoneAborption() const noexcept;
    float getOzoneHeightCenter() const noexcept;
    float getOzoneHeightRange() const noexcept;

    void setPlanetRadius(float planetRadius) noexcept;
    void setAtmosphereRadius(float atmosphereRadius) noexcept;
    void setRayleighScattering(Vector3f const &rayleighScattering) noexcept;
    void setRayleighScaleHeight(float rayleighScaleHeight) noexcept;
    void setMieScattering(float mieScattering) noexcept;
    void setMieAbsorption(float mieAbsorption) noexcept;
    void setMieG(float mieG) noexcept;
    void setMieScaleHeight(float mieScaleHeight) noexcept;
    void setOzoneAbsorption(Vector3f const &ozoneAbsorption) noexcept;
    void setOzoneHeightCenter(float ozoneHeightCenter) noexcept;
    void setOzoneHeightRange(float ozoneHeightRange) noexcept;

  private:
    float planetRadius_;
    float atmosphereRadius_;
    Vector3f rayleighScattering_;
    float rayleighScaleHeight_;
    float mieScattering_;
    float mieAbsorption_;
    float mieG_;
    float mieScaleHeight_;
    Vector3f ozoneAbsorption_;
    float ozoneHeightCenter_;
    float ozoneHeightRange_;
  };
} // namespace imp