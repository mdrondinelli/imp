#include "scattering_lut.h"

namespace imp {
  scattering_lut::scattering_lut(gpu_context &context, vector3u const &size):
      size_{size},
      image_{create_image(context, size)},
      image_view_{create_image_view(context)} {}

  gpu_image
  scattering_lut::create_image(gpu_context &context, vector3u const &size) {
    auto image_info = vk::ImageCreateInfo{};
    image_info.imageType = vk::ImageType::e3D;
    image_info.format = vk::Format::eR16G16B16A16Sfloat;
    image_info.extent.width = size[0];
    image_info.extent.height = size[1];
    image_info.extent.depth = size[2];
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = vk::SampleCountFlagBits::e1;
    image_info.tiling = vk::ImageTiling::eOptimal;
    image_info.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
    image_info.sharingMode = vk::SharingMode::eExclusive;
    image_info.initialLayout = vk::ImageLayout::eUndefined;
    auto alloc_info = VmaAllocationCreateInfo{};
    alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return context.create_image(image_info, alloc_info);
  }

  vk::UniqueImageView scattering_lut::create_image_view(gpu_context &context) {
    auto view_info = vk::ImageViewCreateInfo{};
    view_info.image = *image_;
    view_info.viewType = vk::ImageViewType::e3D;
    view_info.format = vk::Format::eR16G16B16A16Sfloat;
    view_info.components.r = vk::ComponentSwizzle::eIdentity;
    view_info.components.g = vk::ComponentSwizzle::eIdentity;
    view_info.components.b = vk::ComponentSwizzle::eIdentity;
    view_info.components.a = vk::ComponentSwizzle::eIdentity;
    view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    return context.device().createImageViewUnique(view_info);
  }

  vector3u const &scattering_lut::size() const noexcept {
    return size_;
  }

  vk::Image scattering_lut::image() const noexcept {
    return *image_;
  }

  vk::ImageView scattering_lut::image_view() const noexcept {
    return *image_view_;
  }
} // namespace imp