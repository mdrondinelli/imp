#include "Renderer.h"

#include <fstream>

#include "../math/Matrix.h"

namespace imp {
  namespace {
    constexpr auto const FRAME_CONCURRENCY = 3u;
  }

  Renderer::Renderer(RendererCreateInfo const &createInfo):
      window_{createInfo.window},
      transmittanceLutFlyweight_{createInfo.window->getContext()},
      skyViewLutFlyweight_{createInfo.window->getContext()},
      frameFlyweight_{
          createInfo.window,
          &transmittanceLutFlyweight_,
          &skyViewLutFlyweight_},
      frames_{createFrames()} {}

  Renderer::~Renderer() {
    window_->getContext()->getDevice().waitIdle();
  }

  void Renderer::render(Scene const &scene) {
    auto &frame = frames_[frame_ % frames_.size()];
    frame.render(scene, frame_);
    ++frame_;
  }

  std::vector<Frame> Renderer::createFrames() {
    auto frames = std::vector<Frame>{};
    for (auto i = 0u; i < FRAME_CONCURRENCY; ++i) {
      frames.emplace_back(&frameFlyweight_);
    }
    return frames;
  }
} // namespace imp