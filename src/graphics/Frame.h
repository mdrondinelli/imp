#pragma once

#include "../core/Window.h"
#include "FrameFlyweight.h"
#include "Scene.h"
#include "SkyViewLut.h"
#include "TransmittanceLut.h"

namespace imp {
  struct FrameCreateInfo {
    Vector2u transmittanceLutSize = {256u, 64u};
    Vector2u skyViewLutSize = {128u, 64u};
  };

  class Frame {
  public:
    Frame(
        std::shared_ptr<FrameFlyweight const> flyweight,
        FrameCreateInfo const &createInfo);

    void render(Scene const &scene, std::uint32_t seed);

  private:
    std::shared_ptr<FrameFlyweight const> flyweight_;
    vk::UniqueSemaphore imageAcquisitionSemaphore_;
    vk::UniqueSemaphore queueSubmissionSemaphore_;
    vk::UniqueFence queueSubmissionFence_;
    vk::UniqueCommandPool commandPool_;
    vk::UniqueCommandBuffer commandBuffer_;
    TransmittanceLut transmittanceLut_;
    SkyViewLut skyViewLut_;

    void computeTransmittanceLut(Scene const &scene);
    void computeSkyViewLut(Scene const &scene);
    void renderAtmosphere(
        Scene const &scene, std::uint32_t seed, vk::Framebuffer framebuffer);

    vk::UniqueCommandPool createCommandPool();
    vk::UniqueCommandBuffer allocateCommandBuffer();
  };
} // namespace imp