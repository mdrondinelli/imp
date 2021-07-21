#include "Planet.h"

#include "../util/Gsl.h"

namespace imp {
  Planet::Planet(
      Eigen::Vector3f const &position,
      float groundRadius,
      float atmosphereRadius,
      Spectrum const &albedo,
      Spectrum const &rayleighScattering,
      float rayleighScaleHeight,
      float mieScattering,
      float mieAbsorption,
      float mieScaleHeight,
      float mieG,
      Spectrum const &ozoneAbsorption,
      float ozoneLayerHeight,
      float ozoneLayerThickness) noexcept:
      position_{position},
      groundRadius_{groundRadius},
      atmosphereRadius_{atmosphereRadius},
      albedo_{albedo},
      rayleighScattering_{rayleighScattering},
      rayleighScaleHeight_{rayleighScaleHeight},
      mieScattering_{mieScattering},
      mieAbsorption_{mieAbsorption},
      mieScaleHeight_{mieScaleHeight},
      mieG_{mieG},
      ozoneAbsorption_{ozoneAbsorption},
      ozoneLayerHeight_{ozoneLayerHeight},
      ozoneLayerThickness_{ozoneLayerThickness} {}

  void Planet::store(char *dst) const noexcept {
    std::memcpy(dst + 0, position_.data(), 12);
    std::memcpy(dst + 12, &groundRadius_, 4);
    std::memcpy(dst + 16, &atmosphereRadius_, 4);
    std::memcpy(dst + 32, albedo_.data(), 12);
    std::memcpy(dst + 48, rayleighScattering_.data(), 12);
    std::memcpy(dst + 60, &rayleighScaleHeight_, 4);
    std::memcpy(dst + 64, &mieScattering_, 4);
    std::memcpy(dst + 68, &mieAbsorption_, 4);
    std::memcpy(dst + 72, &mieScaleHeight_, 4);
    std::memcpy(dst + 76, &mieG_, 4);
    std::memcpy(dst + 80, ozoneAbsorption_.data(), 12);
    std::memcpy(dst + 92, &ozoneLayerHeight_, 4);
    std::memcpy(dst + 96, &ozoneLayerThickness_, 4);
  }

  Eigen::Vector3f const &Planet::getPosition() const noexcept {
    return position_;
  }

  void Planet::setPosition(Eigen::Vector3f const &position) noexcept {
    position_ = position;
  }

  float Planet::getGroundRadius() const noexcept {
    return groundRadius_;
  }

  void Planet::setGroundRadius(float groundRadius) noexcept {
    Expects(groundRadius>= 0.0f);
    groundRadius_ = groundRadius;
  }

  float Planet::getAtmosphereRadius() const noexcept {
    return atmosphereRadius_;
  }

  void Planet::setAtmosphereRadius(float atmosphereRadius) noexcept {
    Expects(atmosphereRadius >= 0.0f);
    atmosphereRadius_ = atmosphereRadius;
  }

  Spectrum const& Planet::getAlbedo() const noexcept {
    return albedo_;
  }

  void Planet::setAlbedo(Spectrum const& albedo) noexcept {
    albedo_ = albedo;
  }

  Spectrum const &Planet::getRayleighScattering() const noexcept {
    return rayleighScattering_;
  }

  void Planet::setRayleighScattering(Spectrum const &scattering) noexcept {
    Expects(scattering(0) >= 0.0f);
    Expects(scattering(1) >= 0.0f);
    Expects(scattering(2) >= 0.0f);
    rayleighScattering_ = scattering;
  }

  float Planet::getRayleighScaleHeight() const noexcept {
    return rayleighScaleHeight_;
  }

  void Planet::setRayleighScaleHeight(float scaleHeight) noexcept {
    Expects(scaleHeight > 0.0f);
    rayleighScaleHeight_ = scaleHeight;
  }

  float Planet::getMieScattering() const noexcept {
    return mieScattering_;
  }

  void Planet::setMieScattering(float mieScattering) noexcept {
    Expects(mieScattering >= 0.0f);
    mieScattering_ = mieScattering;
  }

  float Planet::getMieAbsorption() const noexcept {
    return mieAbsorption_;
  }

  void Planet::setMieAbsorption(float mieAbsorption) noexcept {
    Expects(mieAbsorption >= 0.0f);
    mieAbsorption_ = mieAbsorption;
  }

  float Planet::getMieScaleHeight() const noexcept {
    return mieScaleHeight_;
  }

  void Planet::setMieScaleHeight(float scaleHeight) noexcept {
    Expects(scaleHeight > 0.0f);
    mieScaleHeight_ = scaleHeight;
  }

  float Planet::getMieG() const noexcept {
    return mieG_;
  }

  void Planet::setMieG(float g) noexcept {
    Expects(std::abs(g) < 1.0f);
    mieG_ = g;
  }

  Spectrum const &Planet::getOzoneAbsorption() const noexcept {
    return ozoneAbsorption_;
  }

  void Planet::setOzoneAbsorption(Spectrum const &absorption) noexcept {
    Expects(absorption(0) >= 0.0f);
    Expects(absorption(1) >= 0.0f);
    Expects(absorption(2) >= 0.0f);
    ozoneAbsorption_ = absorption;
  }

  float Planet::getOzoneLayerHeight() const noexcept {
    return ozoneLayerHeight_;
  }

  void Planet::setOzoneLayerHeight(float height) noexcept {
    Expects(height >= 0.0f);
    ozoneLayerHeight_ = height;
  }

  float Planet::getOzoneLayerThickness() const noexcept {
    return ozoneLayerThickness_;
  }

  void Planet::setOzoneLayerThickness(float thickness) noexcept {
    Expects(thickness >= 0.0f);
    ozoneLayerThickness_ = thickness;
  }
} // namespace imp