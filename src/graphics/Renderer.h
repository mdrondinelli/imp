#pragma once

#include <filesystem>

#include "../core/Window.h"
#include "Scene.h"
#include "Frame.h"

namespace imp {
  struct RendererCreateInfo {
    Window *window;
  };

  class Renderer {
  public:
    Renderer(RendererCreateInfo const &createInfo);
    ~Renderer();

    void render(Scene const &scene);

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