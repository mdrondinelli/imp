#include "Atmosphere.h"

#include "../core/GpuContext.h"

namespace imp {
  Atmosphere::Atmosphere(
      Spectrum const &rayleighScattering,
      float rayleighScaleHeight,
      float mieScattering,
      float mieAbsorption,
      float mieScaleHeight,
      float mieG,
      Spectrum const &ozoneAbsorption,
      float ozoneLayerHeight,
      float ozoneLayerThickness,
      float planetRadius,
      float atmosphereRadius) noexcept:
      rayleighScattering_{rayleighScattering},
      rayleighScaleHeight_{rayleighScaleHeight},
      mieScattering_{mieScattering},
      mieAbsorption_{mieAbsorption},
      mieScaleHeight_{mieScaleHeight},
      mieG_{mieG},
      ozoneAbsorption_{ozoneAbsorption},
      ozoneLayerHeight_{ozoneLayerHeight},
      ozoneLayerThickness_{ozoneLayerThickness},
      planetRadius_{planetRadius},
      atmosphereRadius_{atmosphereRadius} {}

  Spectrum const &Atmosphere::getRayleighScattering() const noexcept {
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

  Spectrum const &Atmosphere::getOzoneAbsorption() const noexcept {
    return ozoneAbsorption_;
  }

  float Atmosphere::getOzoneLayerHeight() const noexcept {
    return ozoneLayerHeight_;
  }

  float Atmosphere::getOzoneLayerThickness() const noexcept {
    return ozoneLayerThickness_;
  }

  float Atmosphere::getPlanetRadius() const noexcept {
    return planetRadius_;
  }

  float Atmosphere::getAtmosphereRadius() const noexcept {
    return atmosphereRadius_;
  }

  void Atmosphere::setRayleighScattering(Spectrum const &scattering) noexcept {
    rayleighScattering_ = scattering;
  }

  void Atmosphere::setRayleighScaleHeight(float scaleHeight) noexcept {
    rayleighScaleHeight_ = scaleHeight;
  }

  void Atmosphere::setMieScattering(float mieScattering) noexcept {
    mieScattering_ = mieScattering;
  }

  void Atmosphere::setMieAbsorption(float mieAbsorption) noexcept {
    mieAbsorption_ = mieAbsorption;
  }

  void Atmosphere::setMieScaleHeight(float scaleHeight) noexcept {
    mieScaleHeight_ = scaleHeight;
  }

  void Atmosphere::setMieG(float g) noexcept {
    mieG_ = g;
  }

  void
  Atmosphere::setOzoneAbsorption(Spectrum const &absorption) noexcept {
    ozoneAbsorption_ = absorption;
  }

  void Atmosphere::setOzoneLayerHeight(float height) noexcept {
    ozoneLayerHeight_ = height;
  }

  void Atmosphere::setOzoneLayerThickness(float thickness) noexcept {
    ozoneLayerThickness_ = thickness;
  }

  void Atmosphere::setPlanetRadius(float planetRadius) noexcept {
    planetRadius_ = planetRadius;
  }

  void Atmosphere::setAtmosphereRadius(float atmosphereRadius) noexcept {
    atmosphereRadius_ = atmosphereRadius;
  }
} // namespace imp