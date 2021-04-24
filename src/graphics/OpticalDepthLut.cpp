#include "OpticalDepthLut.h"

namespace imp {
  OpticalDepthLut::OpticalDepthLut(OpticalDepthLutCreateInfo const &createInfo):
      size_{createInfo.size},
      image_{createImage(*createInfo.context)},
      imageView_{createImageView(*createInfo.context)} {}

  Vector2u const &OpticalDepthLut::getSize() const noexcept {
    return size_;
  }

  vk::Image OpticalDepthLut::getImage() const noexcept {
    return *image_;
  }

  vk::ImageView OpticalDepthLut::getImageView() const noexcept {
    return *imageView_;
  }

  GpuImage OpticalDepthLut::createImage(GpuContext &context) {
    auto imageInfo = vk::ImageCreateInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = vk::Format::eR16G16B16A16Sfloat;
    imageInfo.extent.width = size_[0];
    imageInfo.extent.height = size_[1];
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    auto allocInfo = VmaAllocationCreateInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return context.createImage(imageInfo, allocInfo);
  }

  vk::UniqueImageView OpticalDepthLut::createImageView(GpuContext &context) {
    auto viewInfo = vk::ImageViewCreateInfo{};
    viewInfo.image = *image_;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = vk::Format::eR16G16B16A16Sfloat;
    viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.a = vk::ComponentSwizzle::eIdentity;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    return context.getDevice().createImageViewUnique(viewInfo);
  }
} // namespace imp