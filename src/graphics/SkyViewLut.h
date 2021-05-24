#pragma once

#include "TransmittanceLut.h"
#include "SkyViewLutFlyweight.h"

namespace imp {
  class SkyViewLut {
  public:
    SkyViewLut(
        std::shared_ptr<SkyViewLutFlyweight const> flyweight,
        Vector2u const &size);

    Vector2u const &getSize() const noexcept;
    vk::Image getImage() const noexcept;
    vk::ImageView getImageView() const noexcept;
    vk::DescriptorSet getImageDescriptorSet() const noexcept;
    vk::DescriptorSet getTextureDescriptorSet() const noexcept;

    void compute(
        vk::CommandBuffer cmd,
        Scene const &scene,
        TransmittanceLut const &transmittanceLut);

  private:
    std::shared_ptr<SkyViewLutFlyweight const> flyweight_;
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