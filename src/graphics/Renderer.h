#pragma once

#include "Frame.h"

namespace imp {
  class Camera;
  class Scene;
  class Window;

  class Renderer {
  public:
    Renderer(Window *window);
    ~Renderer();

    void render(Scene const &scene, Camera const &camera);

  private:
    Window *window_;
    TransmittanceLut::Flyweight transmittanceLutFlyweight_;
    SkyViewLut::Flyweight skyViewLutFlyweight_;
    Frame::Flyweight frameFlyweight_;
    std::vector<Frame> frames_;
    size_t frame_;

    std::vector<Frame> createFrames();
  };
} // namespace imp