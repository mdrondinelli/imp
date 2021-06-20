#pragma once

#include "Frame.h"

namespace imp {
  class Camera;
  class Scene;
  class Display;

  class Renderer {
  public:
    explicit Renderer(Display *display);
    ~Renderer();

    //void beginFrame();
    //void endFrame();


    void render(Scene const &scene, Camera const &camera);

  private:
    Display *display_;
    TransmittanceLut::Flyweight transmittanceLutFlyweight_;
    SkyViewLut::Flyweight skyViewLutFlyweight_;
    Frame::Flyweight frameFlyweight_;
    std::vector<Frame> frames_;
    size_t frame_;

    std::vector<Frame> createFrames();
  };
} // namespace imp