#include "Atmosphere.h"

#include "../core/GpuContext.h"

namespace imp {
  Atmosphere::Atmosphere(
      float planetRadius,
      float atmosphereRadius,
      Vector3f const &rayleighScattering,
      float rayleighScaleHeight,
      float mieScattering,
      float mieAbsorption,
      float mieScaleHeight,
      float mieG,
      Vector3f const &ozoneAbsorption,
      float ozoneLayerHeight,
      float ozoneLayerThickness) noexcept:
      planetRadius_{planetRadius},
      atmosphereRadius_{atmosphereRadius},
      rayleighScattering_{rayleighScattering},
      rayleighScaleHeight_{rayleighScaleHeight},
      mieScattering_{mieScattering},
      mieAbsorption_{mieAbsorption},
      mieScaleHeight_{mieScaleHeight},
      mieG_{mieG},
      ozoneAbsorption_{ozoneAbsorption},
      ozoneLayerHeight_{ozoneLayerHeight},
      ozoneLayerThickness_{ozoneLayerThickness} {}

  float Atmosphere::getPlanetRadius() const noexcept {
    return planetRadius_;
  }

  float Atmosphere::getAtmosphereRadius() const noexcept {
    return atmosphereRadius_;
  }

  Vector3f const &Atmosphere::getRayleighScattering() const noexcept {
    return rayleighScattering_;
  }

  float Atmosphere::getRayleighScaleHeight() const noexcept {
    return rayleighScaleHeight_;
  }

  float Atmosphere::getMieScattering() const noexcept {
    return mieScattering_;
  }

  float Atmosphere::getMieAbsorption() const noexcept {
    return mieAbsorption_;
  }

  float Atmosphere::getMieScaleHeight() const noexcept {
    return mieScaleHeight_;
  }

  float Atmosphere::getMieG() const noexcept {
    return mieG_;
  }

  Vector3f const &Atmosphere::getOzoneAbsorption() const noexcept {
    return ozoneAbsorption_;
  }

  float Atmosphere::getOzoneLayerHeight() const noexcept {
    return ozoneLayerHeight_;
  }

  float Atmosphere::getOzoneLayerThickness() const noexcept {
    return ozoneLayerThickness_;
  }

  void Atmosphere::setPlanetRadius(float planetRadius) noexcept {
    planetRadius_ = planetRadius;
  }

  void Atmosphere::setAtmosphereRadius(float atmosphereRadius) noexcept {
    atmosphereRadius_ = atmosphereRadius;
  }

  void Atmosphere::setRayleighScattering(
      Vector3f const &rayleighScattering) noexcept {
    rayleighScattering_ = rayleighScattering;
  }

  void Atmosphere::setRayleighScaleHeight(float rayleighScaleHeight) noexcept {
    rayleighScaleHeight_ = rayleighScaleHeight;
  }

  void Atmosphere::setMieScattering(float mieScattering) noexcept {
    mieScattering_ = mieScattering;
  }

  void Atmosphere::setMieAbsorption(float mieAbsorption) noexcept {
    mieAbsorption_ = mieAbsorption;
  }

  void Atmosphere::setMieG(float mieG) noexcept {
    mieG_ = mieG;
  }

  void Atmosphere::setMieScaleHeight(float mieScaleHeight) noexcept {
    mieScaleHeight_ = mieScaleHeight;
  }

  void
  Atmosphere::setOzoneAbsorption(Vector3f const &ozoneAbsorption) noexcept {
    ozoneAbsorption_ = ozoneAbsorption;
  }

  void Atmosphere::setOzoneLayerHeight(float ozoneLayerHeight) noexcept {
    ozoneLayerHeight_ = ozoneLayerHeight;
  }

  void Atmosphere::setOzoneLayerThickness(float ozoneLayerThickness) noexcept {
    ozoneLayerThickness_ = ozoneLayerThickness;
  }
} // namespace imp