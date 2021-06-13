#include "Renderer.h"

#include "../system/Display.h"

namespace imp {
  namespace {
    constexpr auto const FRAME_CONCURRENCY = 3u;
  }

  Renderer::Renderer(Display *display):
      display_{display},
      transmittanceLutFlyweight_{display->getContext()},
      skyViewLutFlyweight_{display->getContext()},
      frameFlyweight_{
          display,
          &transmittanceLutFlyweight_,
          &skyViewLutFlyweight_},
      frames_{createFrames()} {}

  Renderer::~Renderer() {
    display_->getContext()->getDevice().waitIdle();
  }

  void Renderer::render(Scene const &scene, Camera const &camera) {
    auto &frame = frames_[frame_ % frames_.size()];
    frame.render(scene, camera, frame_);
    ++frame_;
  }

  std::vector<Frame> Renderer::createFrames() {
    auto frames = std::vector<Frame>{};
    for (auto i = 0u; i < FRAME_CONCURRENCY; ++i) {
      frames.emplace_back(&frameFlyweight_, 256, 64, 128, 128);
    }
    return frames;
  }
} // namespace imp