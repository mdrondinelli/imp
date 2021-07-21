#include "GpuImage.h"

#include "GpuContext.h"

namespace imp {
  GpuImage::GpuImage(
      gsl::not_null<VmaAllocator> allocator,
      vk::ImageCreateInfo const &imageCreateInfo,
      VmaAllocationCreateInfo const &allocationCreateInfo):
      allocator_{allocator},
      flags_{imageCreateInfo.flags},
      type_{imageCreateInfo.imageType},
      format_{imageCreateInfo.format},
      extent_{imageCreateInfo.extent},
      mipLevels_{imageCreateInfo.mipLevels},
      arrayLayers_{imageCreateInfo.arrayLayers},
      samples_{imageCreateInfo.samples},
      tiling_{imageCreateInfo.tiling},
      usage_{imageCreateInfo.usage},
      sharingMode_{imageCreateInfo.sharingMode} {
    if (vmaCreateImage(
            allocator_,
            reinterpret_cast<VkImageCreateInfo const *>(&imageCreateInfo),
            &allocationCreateInfo,
            reinterpret_cast<VkImage *>(&image_),
            &allocation_,
            nullptr) != VK_SUCCESS) {
      throw std::runtime_error{"failed to create gpu image."};
    }
  }

  GpuImage::~GpuImage() {
    vmaDestroyImage(allocator_, image_, allocation_);
  }

  GpuImage::GpuImage(GpuImage &&rhs) noexcept:
      allocator_{rhs.allocator_},
      flags_{rhs.flags_},
      type_{rhs.type_},
      format_{rhs.format_},
      extent_{rhs.extent_},
      mipLevels_{rhs.mipLevels_},
      arrayLayers_{rhs.arrayLayers_},
      samples_{rhs.samples_},
      tiling_{rhs.tiling_},
      usage_{rhs.usage_},
      sharingMode_{rhs.sharingMode_},
      image_{rhs.image_},
      allocation_{rhs.allocation_} {
    rhs.image_ = nullptr;
    rhs.allocation_ = nullptr;
  }

  GpuImage &GpuImage::operator=(GpuImage &&rhs) noexcept {
    if (&rhs != this) {
      vmaDestroyImage(allocator_, image_, allocation_);
      allocator_ = rhs.allocator_;
      flags_ = rhs.flags_;
      type_ = rhs.type_;
      format_ = rhs.format_;
      extent_ = rhs.extent_;
      mipLevels_ = rhs.mipLevels_;
      arrayLayers_ = rhs.arrayLayers_;
      samples_ = rhs.samples_;
      tiling_ = rhs.tiling_;
      usage_ = rhs.usage_;
      sharingMode_ = rhs.sharingMode_;
      image_ = rhs.image_;
      allocation_ = rhs.allocation_;
      rhs.image_ = nullptr;
      rhs.allocation_ = nullptr;
    }
    return *this;
  }

  bool GpuImage::hasValue() const noexcept {
    return image_;
  }

  vk::ImageCreateFlags GpuImage::getFlags() const noexcept {
    return flags_;
  }

  vk::ImageType GpuImage::getType() const noexcept {
    return type_;
  }

  vk::Format GpuImage::getFormat() const noexcept {
    return format_;
  }

  Extent3u const &GpuImage::getExtent() const noexcept {
    return extent_;
  }

  std::uint32_t GpuImage::getMipLevels() const noexcept {
    return mipLevels_;
  }

  std::uint32_t GpuImage::getArrayLayers() const noexcept {
    return arrayLayers_;
  }

  vk::SampleCountFlagBits GpuImage::getSamples() const noexcept {
    return samples_;
  }

  vk::ImageTiling GpuImage::getTiling() const noexcept {
    return tiling_;
  }

  vk::ImageUsageFlags GpuImage::getUsage() const noexcept {
    return usage_;
  }

  vk::SharingMode GpuImage::getSharingMode() const noexcept {
    return sharingMode_;
  }

  vk::Image GpuImage::get() const noexcept {
    return image_;
  }
} // namespace imp