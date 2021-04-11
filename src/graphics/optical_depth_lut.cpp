#include "optical_depth_lut.h"

namespace imp {
  optical_depth_lut::optical_depth_lut(
      optical_depth_lut_create_info const &info, gpu_allocator &allocator):
      size_{info.size},
      image_{create_image(info, allocator)},
      image_view_{create_image_view(info, allocator)} {}

  gpu_image optical_depth_lut::create_image(
      optical_depth_lut_create_info const &info, gpu_allocator &allocator) {
    auto image_info = vk::ImageCreateInfo{};
    image_info.imageType = vk::ImageType::e2D;
    image_info.format = vk::Format::eR16G16B16A16Sfloat;
    image_info.extent.width = info.size[0];
    image_info.extent.height = info.size[1];
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = vk::SampleCountFlagBits::e1;
    image_info.tiling = vk::ImageTiling::eOptimal;
    image_info.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
    if (info.graphics_family != info.compute_family) {
      auto indices = std::array{info.graphics_family, info.compute_family};
      image_info.sharingMode = vk::SharingMode::eConcurrent;
      image_info.queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
      image_info.pQueueFamilyIndices = indices.data();
    } else {
      image_info.sharingMode = vk::SharingMode::eExclusive;
    }
    image_info.initialLayout = vk::ImageLayout::eUndefined;
    auto alloc_info = VmaAllocationCreateInfo{};
    alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return allocator.create_image(image_info, alloc_info);
  }

  vk::UniqueImageView optical_depth_lut::create_image_view(
      optical_depth_lut_create_info const &info, gpu_allocator &allocator) {
    auto view_info = vk::ImageViewCreateInfo{};
    view_info.image = image_.get();
    view_info.viewType = vk::ImageViewType::e2D;
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
    return info.device.createImageViewUnique(view_info);
  }

  vector2u const &optical_depth_lut::size() const noexcept {
    return size_;
  }

  vk::Image optical_depth_lut::image() const noexcept {
    return image_.get();
  }

  vk::ImageView optical_depth_lut::image_view() const noexcept {
    return image_view_.get();
  }
} // namespace imp