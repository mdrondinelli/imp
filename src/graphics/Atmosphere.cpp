#include "Atmosphere.h"

namespace imp {
  Atmosphere::Atmosphere(AtmosphereCreateInfo const &createInfo) noexcept:
      planetRadius_{createInfo.planetRadius},
      atmosphereRadius_{createInfo.atmosphereRadius},
      rayleighScattering_{createInfo.rayleighScattering},
      rayleighScaleHeight_{createInfo.rayleighScaleHeight},
      mieScattering_{createInfo.mieScattering},
      mieAbsorption_{createInfo.mieAbsorption},
      mieG_{createInfo.mieG},
      mieScaleHeight_{createInfo.mieScaleHeight},
      ozoneAbsorption_{createInfo.ozoneAbsorption},
      ozoneHeightCenter_{createInfo.ozoneHeightCenter},
      ozoneHeightRange_{createInfo.ozoneHeightRange} {}

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

  float Atmosphere::getMieG() const noexcept {
    return mieG_;
  }

  float Atmosphere::getMieScaleHeight() const noexcept {
    return mieScaleHeight_;
  }

  Vector3f const &Atmosphere::getOzoneAborption() const noexcept {
    return ozoneAbsorption_;
  }

  float Atmosphere::getOzoneHeightCenter() const noexcept {
    return ozoneHeightCenter_;
  }

  float Atmosphere::getOzoneHeightRange() const noexcept {
    return ozoneHeightRange_;
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

  void Atmosphere::setOzoneHeightCenter(float ozoneHeightCenter) noexcept {
    ozoneHeightCenter_ = ozoneHeightCenter;
  }

  void Atmosphere::setOzoneHeightRange(float ozoneHeightRange) noexcept {
    ozoneHeightRange_ = ozoneHeightRange;
  }
} // namespace imp