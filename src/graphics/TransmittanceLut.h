#pragma once

#include "../core/GpuImage.h"
#include "../math/Vector.h"

namespace imp {
  class AtmosphereBuffer;
  class Scene;

  class TransmittanceLut {
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

    TransmittanceLut(
        Flyweight const *flyweight,
        AtmosphereBuffer const *buffer,
        Vector2u const &size);

    Flyweight const *getFlyweight() const noexcept;
    AtmosphereBuffer const *getBuffer() const noexcept;
    Vector2u const &getSize() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;
    vk::DescriptorSet getComputeDescriptorSet() const noexcept;
    vk::DescriptorSet getRenderDescriptorSet() const noexcept;

    bool compute(vk::CommandBuffer cmd, Scene const &scene);

  private:
    Flyweight const *flyweight_;
    AtmosphereBuffer const *buffer_;
    Vector2u size_;
    GpuImage image_;
    vk::UniqueImageView imageView_;
    vk::UniqueDescriptorPool descriptorPool_;
    std::vector<vk::DescriptorSet> descriptorSets_;

    float planetRadius_;
    float atmosphereRadius_;
    Vector3f rayleighScattering_;
    float rayleighScaleHeight_;
    float mieExtinction_;
    float mieScaleHeight_;
    Vector3f ozoneAbsorption_;
    float ozoneLayerHeight_;
    float ozoneLayerThickness_;

    GpuImage createImage();
    vk::UniqueImageView createImageView();
    vk::UniqueDescriptorPool createDescriptorPool();
    std::vector<vk::DescriptorSet> allocateDescriptorSets();
    void updateDescriptorSets();
  };
} // namespace imp