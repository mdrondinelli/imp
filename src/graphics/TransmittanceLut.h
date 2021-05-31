#pragma once

#include <memory>

#include "../core/GpuImage.h"
#include "../math/Vector.h"

namespace imp {
  class Scene;
  class TransmittanceLut {
  public:
    class Flyweight {
    public:
      explicit Flyweight(GpuContext &context);

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

    TransmittanceLut(
        std::shared_ptr<Flyweight const> flyweight, Vector2u const &size);

    Vector2u const &getSize() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;
    vk::DescriptorSet getImageDescriptorSet() const noexcept;
    vk::DescriptorSet getTextureDescriptorSet() const noexcept;

    bool compute(vk::CommandBuffer cmd, Scene const &scene);

  private:
    std::shared_ptr<Flyweight const> flyweight_;
    Vector2u size_;
    GpuImage image_;
    vk::UniqueImageView imageView_;
    vk::UniqueDescriptorPool descriptorPool_;
    std::vector<vk::DescriptorSet> descriptorSets_;

    float planetRadius_;
    float atmosphereRadius_;
    Vector3f rayleighExtinction_;
    float rayleighScaleHeight_;
    float mieExtinction_;
    float mieScaleHeight_;
    Vector3f ozoneExtinction_;
    float ozoneHeightCenter_;
    float ozoneHeightRange_;

    GpuImage createImage();
    vk::UniqueImageView createImageView();
    vk::UniqueDescriptorPool createDescriptorPool();
    std::vector<vk::DescriptorSet> allocateDescriptorSets();
    void updateDescriptorSets();
  };
} // namespace imp