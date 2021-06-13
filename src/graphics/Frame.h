#pragma once

#include "AtmosphereBuffer.h"
#include "SkyViewLut.h"
#include "TransmittanceLut.h"

namespace imp {
  class Scene;
  class Window;

  class Frame {
  public:
    class Flyweight {
    public:
      Flyweight(
          Window *window,
          TransmittanceLut::Flyweight const *transmittanceLutFlyweight,
          SkyViewLut::Flyweight const *skyViewLutFlyweight);

      Window *getWindow() const noexcept;
      TransmittanceLut::Flyweight const *
      getTransmittanceLutFlyweight() const noexcept;
      SkyViewLut::Flyweight const *getSkyViewLutFlyweight() const noexcept;
      vk::RenderPass getRenderPass() const noexcept;
      vk::PipelineLayout getPipelineLayout() const noexcept;
      vk::Pipeline getPipeline() const noexcept;

    private:
      Window *window_;
      TransmittanceLut::Flyweight const *transmittanceLutFlyweight_;
      SkyViewLut::Flyweight const *skyViewLutFlyweight_;
      vk::UniqueRenderPass renderPass_;
      vk::UniquePipelineLayout pipelineLayout_;
      vk::UniquePipeline pipeline_;

      vk::UniqueRenderPass createRenderPass();
      vk::UniquePipelineLayout createPipelineLayout();
      vk::UniquePipeline createPipeline();
    };

    Frame(
        Flyweight const *flyweight,
        unsigned transmittanceLutWidth,
        unsigned transmittanceLutHeight,
        unsigned skyViewLutWidth,
        unsigned skyViewLutHeight);

    void render(Scene const &scene, Camera const &camera, std::uint32_t seed);

  private:
    Flyweight const *flyweight_;
    vk::UniqueSemaphore imageAcquisitionSemaphore_;
    vk::UniqueSemaphore queueSubmissionSemaphore_;
    vk::UniqueFence queueSubmissionFence_;
    vk::UniqueCommandPool commandPool_;
    vk::UniqueCommandBuffer commandBuffer_;
    AtmosphereBuffer atmosphereBuffer_;
    TransmittanceLut transmittanceLut_;
    SkyViewLut skyViewLut_;

    void computeTransmittanceLut(Atmosphere const &atmosphere);
    void computeSkyViewLut(DirectionalLight const &sun, Camera const &camera);
    void renderAtmosphere(
        Camera const &camera, std::uint32_t seed, vk::Framebuffer framebuffer);

    vk::UniqueCommandPool createCommandPool();
    vk::UniqueCommandBuffer allocateCommandBuffer();
    GpuBuffer createAtmosphereBuffer();
  };
} // namespace imp