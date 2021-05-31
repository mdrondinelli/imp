#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include "../core/Window.h"
#include "SkyViewLut.h"
#include "TransmittanceLut.h"

namespace imp {
  struct FrameFlyweightCreateInfo {
    std::shared_ptr<TransmittanceLut::Flyweight const>
        transmittanceLutFlyweight;
    std::shared_ptr<SkyViewLut::Flyweight const> skyViewLutFlyweight;
  };

  class FrameFlyweight {
  public:
    FrameFlyweight(Window &window, FrameFlyweightCreateInfo const &createInfo);

    Window *getWindow() const noexcept;
    std::shared_ptr<TransmittanceLut::Flyweight const>
    getTransmittanceLutFlyweight() const noexcept;
    std::shared_ptr<SkyViewLut::Flyweight const>
    getSkyViewLutFlyweight() const noexcept;
    vk::RenderPass getRenderPass() const noexcept;
    vk::PipelineLayout getPipelineLayout() const noexcept;
    vk::Pipeline getPipeline() const noexcept;

  private:
    Window *window_;
    std::shared_ptr<TransmittanceLut::Flyweight const>
        transmittanceLutFlyweight_;
    std::shared_ptr<SkyViewLut::Flyweight const> skyViewLutFlyweight_;
    vk::UniqueRenderPass renderPass_;
    vk::UniquePipelineLayout pipelineLayout_;
    vk::UniquePipeline pipeline_;

    vk::UniqueRenderPass createRenderPass();
    vk::UniquePipelineLayout createPipelineLayout();
    vk::UniquePipeline createPipeline();
  };
} // namespace imp