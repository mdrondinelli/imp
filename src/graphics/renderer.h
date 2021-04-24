#pragma once

#include <filesystem>

#include "../core/Window.h"
#include "Scene.h"

namespace imp {
  struct RendererCreateInfo {
    GpuContext *context;
    Window *window;
  };

  class Renderer {
  public:
    explicit Renderer(RendererCreateInfo const &createInfo);
    ~Renderer();

    void render(Scene const &scene);

  private:
    struct Frame {
      vk::UniqueSemaphore imageAcquisitionSemaphore;
      vk::UniqueSemaphore queueSubmissionSemaphore;
      vk::UniqueFence queueSubmissionFence;
      vk::UniqueCommandPool commandPool;
      vk::UniqueCommandBuffer commandBuffer;
      vk::DescriptorSet atmosphereDescriptorSet;
    };

    GpuContext *context_;
    Window *window_;
    vk::UniqueRenderPass atmospherePass_;
    vk::UniqueDescriptorSetLayout atmosphereDescriptorSetLayout_;
    vk::UniquePipelineLayout atmospherePipelineLayout_;
    vk::UniquePipeline atmospherePipeline_;
    vk::UniqueDescriptorPool descriptorPool_;
    vk::UniqueSampler lutSampler_;
    std::vector<Frame> frames_;
    size_t frame_;

    void updateAtmosphereDescriptorSet(Frame &frame, Atmosphere const &atmosphere);
    
    vk::UniqueRenderPass createAtmospherePass();
    vk::UniqueDescriptorSetLayout createAtmosphereDescriptorSetLayout();
    vk::UniquePipelineLayout createAtmospherePipelineLayout();
    vk::UniquePipeline createAtmospherePipeline();
    vk::UniqueDescriptorPool createDescriptorPool();
    vk::UniqueSampler createLutSampler();
    std::vector<Frame> createFrames();
  };
} // namespace imp