#include "Renderer.h"

#include <fstream>

#include "../math/Matrix.h"

namespace imp {
  namespace {
    constexpr auto const FRAME_CONCURRENCY = 3u;
  }

  Renderer::Renderer(RendererCreateInfo const &createInfo):
      window_{createInfo.window}, frames_{createFrames()} {}

  Renderer::~Renderer() {
    window_->getContext()->getDevice().waitIdle();
  }

  void Renderer::render(Scene const &scene) {
    auto &frame = frames_[frame_ % frames_.size()];
    frame.render(scene, frame_);
    ++frame_;
  }

  std::vector<Frame> Renderer::createFrames() {
    auto frameFlyweightCreateInfo = FrameFlyweightCreateInfo{};
    frameFlyweightCreateInfo.transmittanceLutFlyweight =
        std::make_shared<TransmittanceLut::Flyweight>(*window_->getContext());
    frameFlyweightCreateInfo.skyViewLutFlyweight =
        std::make_shared<SkyViewLut::Flyweight>(*window_->getContext());
    auto frameFlyweight =
        std::make_shared<FrameFlyweight>(*window_, frameFlyweightCreateInfo);
    auto frames = std::vector<Frame>{};
    for (auto i = 0u; i < FRAME_CONCURRENCY; ++i) {
      frames.emplace_back(frameFlyweight, FrameCreateInfo{});
    }
    return frames;
  }
} // namespace imp