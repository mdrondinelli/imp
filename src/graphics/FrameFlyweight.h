#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include "../core/Window.h"
#include "SkyViewLutFlyweight.h"
#include "TransmittanceLutFlyweight.h"

namespace imp {
  struct FrameFlyweightCreateInfo {
    std::shared_ptr<TransmittanceLutFlyweight const> transmittanceLutFlyweight;
    std::shared_ptr<SkyViewLutFlyweight const> skyViewLutFlyweight;
  };

  class FrameFlyweight {
  public:
    FrameFlyweight(Window &window, FrameFlyweightCreateInfo const &createInfo);

    Window *getWindow() const noexcept;
    std::shared_ptr<TransmittanceLutFlyweight const>
    getTransmittanceLutFlyweight() const noexcept;
    std::shared_ptr<SkyViewLutFlyweight const>
    getSkyViewLutFlyweight() const noexcept;
    vk::RenderPass getRenderPass() const noexcept;
    vk::PipelineLayout getPipelineLayout() const noexcept;
    vk::Pipeline getPipeline() const noexcept;

  private:
    Window *window_;
    std::shared_ptr<TransmittanceLutFlyweight const> transmittanceLutFlyweight_;
    std::shared_ptr<SkyViewLutFlyweight const> skyViewLutFlyweight_;
    vk::UniqueRenderPass renderPass_;
    vk::UniquePipelineLayout pipelineLayout_;
    vk::UniquePipeline pipeline_;

    vk::UniqueRenderPass createRenderPass();
    vk::UniquePipelineLayout createPipelineLayout();
    vk::UniquePipeline createPipeline();
  };
} // namespace imp