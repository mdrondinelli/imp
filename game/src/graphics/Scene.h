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
        align(std::size_t{256}, UNIFORM_BUFFER_SIZE);
    static constexpr auto TRANSMITTANCE_IMAGE_EXTENT = Extent3u{64, 256, 1};

    class Flyweight {
    public:
      explicit Flyweight(
          gsl::not_null<GpuContext *> context, std::size_t frameCount);

    private:
      vk::RenderPass createTransmittanceRenderPass() const;
      vk::DescriptorSetLayout createTransmittanceDescriptorSetLayout() const;
      vk::PipelineLayout createTransmittancePipelineLayout() const;
      vk::Pipeline createTransmittancePipeline() const;
      vk::Sampler createTransmittanceSampler() const;

    public:
      ~Flyweight();

      gsl::not_null<GpuContext *> getContext() const noexcept;
      std::size_t getFrameCount() const noexcept;
      vk::RenderPass getTransmittanceRenderPass() const noexcept;
      vk::DescriptorSetLayout
      getTransmittanceDescriptorSetLayout() const noexcept;
      vk::PipelineLayout getTransmittancePipelineLayout() const noexcept;
      vk::Pipeline getTransmittancePipeline() const noexcept;
      vk::Sampler getTransmittanceSampler() const noexcept;

    private:
      gsl::not_null<GpuContext *> context_;
      std::size_t frameCount_;
      vk::RenderPass transmittanceRenderPass_;
      vk::DescriptorSetLayout transmittanceDescriptorSetLayout_;
      vk::PipelineLayout transmittancePipelineLayout_;
      vk::Pipeline transmittancePipeline_;
      vk::Sampler transmittanceSampler_;
    };

    struct Frame {
      GpuImage transmittanceImage;
      vk::ImageView transmittanceImageView;
      vk::Framebuffer transmittanceFramebuffer;
      vk::DescriptorSet transmittanceDescriptorSet;
      vk::CommandPool commandPool;
      vk::CommandBuffer commandBuffer;
      vk::Semaphore semaphore;

      Frame(GpuImage &&transmittanceImage) noexcept;
    };

    explicit Scene(gsl::not_null<Flyweight const *> flyweight);

  private:
    vk::DescriptorPool createDescriptorPool() const;
    GpuBuffer createUniformBuffer() const;
    std::vector<Frame> createFrames() const;
    GpuImage createTransmittanceImage() const;
    void initTransmittanceImageView(Frame &frame) const;
    void initTransmittanceFramebuffer(Frame &frame) const;
    void initTransmittanceDescriptorSet(Frame &frame) const;
    void updateTransmittanceDescriptorSet(Frame &frame, std::size_t index) const;
    void initCommandPool(Frame &frame) const;
    void initCommandBuffer(Frame &frame) const;
    void updateCommandBuffer(Frame &frame) const;
    void initSemaphore(Frame &frame) const;

  public:
    ~Scene();

    void render(std::size_t frameIndex);

  private:
    void updateUniformBuffer(std::size_t frameIndex);

  public:
    gsl::not_null<Flyweight const *> getFlyweight() const noexcept;
    GpuBuffer const &getUniformBuffer() const noexcept;
    GpuImage const &getTransmittanceImage(std::size_t i) const noexcept;
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
    vk::DescriptorPool descriptorPool_;
    GpuBuffer uniformBuffer_;
    std::vector<Frame> frames_;
    std::shared_ptr<Planet> planet_;
    std::shared_ptr<DirectionalLight> sunLight_;
    std::shared_ptr<DirectionalLight> moonLight_;
    bool firstFrame_;
  };
} // namespace imp