#pragma once

#include "../core/GpuImage.h"
#include "Scene.h"
#include "TransmittanceLutFlyweight.h"

namespace imp {
  class TransmittanceLut {
  public:
    TransmittanceLut(
        std::shared_ptr<TransmittanceLutFlyweight const> flyweight,
        Vector2u const &size);

    Vector2u const &getSize() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;
    vk::DescriptorSet getImageDescriptorSet() const noexcept;
    vk::DescriptorSet getTextureDescriptorSet() const noexcept;

    void compute(vk::CommandBuffer cmd, Scene const &scene);

  private:
    std::shared_ptr<TransmittanceLutFlyweight const> flyweight_;
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