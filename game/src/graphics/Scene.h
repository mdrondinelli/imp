#pragma once

#include <memory>

#include "Atmosphere.h"
#include "DirectionalLight.h"

namespace imp {
  class Scene {
  public:
    std::shared_ptr<Atmosphere> getAtmosphere() const noexcept;
    std::shared_ptr<DirectionalLight> getSunLight() const noexcept;
    std::shared_ptr<DirectionalLight> getMoonLight() const noexcept;

    void setAtmosphere(std::shared_ptr<Atmosphere> atmosphere) noexcept;
    void setSunLight(std::shared_ptr<DirectionalLight> sunLight) noexcept;
    void setMoonLight(std::shared_ptr<DirectionalLight> moonLight) noexcept;

  private:
    std::shared_ptr<Atmosphere> atmosphere_;
    std::shared_ptr<DirectionalLight> sunLight_;
    std::shared_ptr<DirectionalLight> moonLight_;
  };
} // namespace imp