#pragma once

#include <memory>
#include <vector>

#include "../system/GpuBuffer.h"
#include "../system/GpuImage.h"
#include "../util/Align.h"
#include "DirectionalLight.h"
#include "Planet.h"

namespace imp {
  class GpuContext;

  class Scene {
  public:
    static constexpr auto UNIFORM_BUFFER_SIZE =
        align(DirectionalLight::UNIFORM_ALIGN, Planet::UNIFORM_SIZE) +
        DirectionalLight::UNIFORM_SIZE;
    static constexpr auto UNIFORM_BUFFER_STRIDE =
        align<256>(UNIFORM_BUFFER_SIZE);
    static constexpr auto TRANSMITTANCE_IMAGE_EXTENT = Extent3u{64, 256, 1};

    class Flyweight {
    public:
      explicit Flyweight(
          gsl::not_null<GpuContext *> context, std::size_t frameCount);

    private:
      vk::DescriptorSetLayout createTransmittanceDescriptorSetLayout() const;
      vk::PipelineLayout createTransmittancePipelineLayout() const;
      vk::Pipeline createTransmittancePipeline() const;
      vk::Sampler createTransmittanceSampler() const;

    public:
      ~Flyweight();

      gsl::not_null<GpuContext *> getContext() const noexcept;
      std::size_t getFrameCount() const noexcept;
      vk::DescriptorSetLayout
      getTransmittanceDescriptorSetLayout() const noexcept;
      vk::PipelineLayout getTransmittancePipelineLayout() const noexcept;
      vk::Pipeline getTransmittancePipeline() const noexcept;
      vk::Sampler getTransmittanceSampler() const noexcept;

    private:
      gsl::not_null<GpuContext *> context_;
      std::size_t frameCount_;
      vk::DescriptorSetLayout transmittanceDescriptorSetLayout_;
      vk::PipelineLayout transmittancePipelineLayout_;
      vk::Pipeline transmittancePipeline_;
      vk::Sampler transmittanceSampler_;
    };

    struct Frame {
      vk::ImageView transmittanceImageView;
      vk::DescriptorSet transmittanceDescriptorSet;
      vk::CommandBuffer commandBuffer;
      vk::Semaphore semaphore;
    };

    explicit Scene(gsl::not_null<Flyweight const *> flyweight);

  private:
    GpuBuffer createUniformBuffer() const;
    GpuImage createTransmittanceImage() const;
    vk::DescriptorPool createDescriptorPool() const;
    vk::CommandPool createCommandPool() const;
    vk::CommandBuffer createTransitionCommandBuffer();
    vk::Semaphore createTransitionSemaphore() const;
    vk::Fence createTransitionFence() const;
    void initTransmittanceImageViews();
    void initTransmittaceDescriptorSets();
    void initCommandBuffers();
    void initSemaphores();

  public:
    ~Scene();

    void render(std::size_t frameIndex);

  private:
    void updateUniformBuffer(std::size_t frameIndex);

  public:
    gsl::not_null<Flyweight const *> getFlyweight() const noexcept;
    GpuBuffer const &getUniformBuffer() const noexcept;
    GpuImage const &getTransmittanceImage() const noexcept;
    vk::ImageView getTransmittanceImageView(std::size_t i) const noexcept;
    vk::Semaphore getSemaphore(std::size_t i) const noexcept;
    std::shared_ptr<Planet> getPlanet() const noexcept;
    void setPlanet(std::shared_ptr<Planet> planet) noexcept;
    std::shared_ptr<DirectionalLight> getSunLight() const noexcept;
    void setSunLight(std::shared_ptr<DirectionalLight> sunLight) noexcept;
    std::shared_ptr<DirectionalLight> getMoonLight() const noexcept;
    void setMoonLight(std::shared_ptr<DirectionalLight> moonLight) noexcept;

  private:
    gsl::not_null<Flyweight const *> flyweight_;
    GpuBuffer uniformBuffer_;
    GpuImage transmittanceImage_;
    vk::DescriptorPool descriptorPool_;
    vk::CommandPool commandPool_;
    vk::CommandBuffer transitionCommandBuffer_;
    vk::Semaphore transitionSemaphore_;
    vk::Fence transitionFence_;
    std::vector<Frame> frames_;
    std::shared_ptr<Planet> planet_;
    std::shared_ptr<DirectionalLight> sunLight_;
    std::shared_ptr<DirectionalLight> moonLight_;
    bool firstFrame_;
  };
} // namespace imp