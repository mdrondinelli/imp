#pragma once

#include "../core/GpuContext.h"

namespace imp {
  class TransmittanceLutFlyweight {
  public:
    explicit TransmittanceLutFlyweight(GpuContext &context);

    GpuContext &getContext() const noexcept;
    vk::DescriptorSetLayout getImageDescriptorSetLayout() const noexcept;
    vk::DescriptorSetLayout getTextureDescriptorSetLayout() const noexcept;
    vk::PipelineLayout getPipelineLayout() const noexcept;
    vk::Pipeline getPipeline() const noexcept;
    vk::Sampler getSampler() const noexcept;

  private:
    GpuContext *context_;
    vk::DescriptorSetLayout imageDescriptorSetLayout_;
    vk::DescriptorSetLayout textureDescriptorSetLayout_;
    vk::UniquePipelineLayout pipelineLayout_;
    vk::UniquePipeline pipeline_;
    vk::Sampler sampler_;

    vk::DescriptorSetLayout createImageDescriptorSetLayout();
    vk::DescriptorSetLayout createTextureDescriptorSetLayout();
    vk::UniquePipelineLayout createPipelineLayout();
    vk::UniquePipeline createPipeline();
    vk::Sampler createSampler();
  };
}