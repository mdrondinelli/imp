#pragma once

#include "../core/GpuImage.h"
#include "../math/Vector.h"

namespace imp {
  class AtmosphereBuffer;
  class Camera;
  class DirectionalLight;
  class TransmittanceLut;

  class SkyViewLut {
  public:
    class Flyweight {
    public:
      explicit Flyweight(GpuContext *context);

      GpuContext *getContext() const noexcept;
      vk::DescriptorSetLayout getComputeDescriptorSetLayout() const noexcept;
      vk::DescriptorSetLayout getRenderDescriptorSetLayout() const noexcept;
      vk::PipelineLayout getPipelineLayout() const noexcept;
      vk::Pipeline getPipeline() const noexcept;
      vk::Sampler getSampler() const noexcept;

    private:
      GpuContext *context_;
      vk::DescriptorSetLayout computeDescriptorSetLayout_;
      vk::DescriptorSetLayout renderDescriptorSetLayout_;
      vk::UniquePipelineLayout pipelineLayout_;
      vk::UniquePipeline pipeline_;
      vk::Sampler sampler_;

      vk::DescriptorSetLayout createComputeDescriptorSetLayout();
      vk::DescriptorSetLayout createRenderDescriptorSetLayout();
      vk::UniquePipelineLayout createPipelineLayout();
      vk::UniquePipeline createPipeline();
      vk::Sampler createSampler();
    };

    SkyViewLut(
        Flyweight const *flyweight,
        TransmittanceLut const *transmittanceLut,
        Vector2u const &size);

    Flyweight const *getFlyweight() const noexcept;
    AtmosphereBuffer const *getBuffer() const noexcept;
    TransmittanceLut const *getTransmittanceLut() const noexcept;
    Vector2u const &getSize() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;
    vk::DescriptorSet getComputeDescriptorSet() const noexcept;
    vk::DescriptorSet getRenderDescriptorSet() const noexcept;

    void compute(
        vk::CommandBuffer cmd,
        DirectionalLight const &sun,
        Camera const &camera);

  private:
    Flyweight const *flyweight_;
    AtmosphereBuffer const *buffer_;
    TransmittanceLut const *transmittanceLut_;
    Vector2u size_;
    GpuImage image_;
    vk::UniqueImageView imageView_;
    vk::UniqueDescriptorPool descriptorPool_;
    std::vector<vk::DescriptorSet> descriptorSets_;

    GpuImage createImage();
    vk::UniqueImageView createImageView();
    vk::UniqueDescriptorPool createDescriptorPool();
    std::vector<vk::DescriptorSet> allocateDescriptorSets();
    void updateDescriptorSets();
  };
} // namespace imp