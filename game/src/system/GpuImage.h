#pragma once

#include <vulkan/vulkan.hpp>

#include "../util/Extent.h"
#include "../util/Gsl.h"
#include "vk_mem_alloc.h"

namespace imp {
  class GpuImage {
  public:
    explicit GpuImage(
        gsl::not_null<VmaAllocator> allocator,
        vk::ImageCreateInfo const &imageCreateInfo,
        VmaAllocationCreateInfo const &allocationCreateInfo);
    ~GpuImage();

    GpuImage(GpuImage &&rhs) noexcept;
    GpuImage &operator=(GpuImage &&rhs) noexcept;

    bool hasValue() const noexcept;
    vk::ImageCreateFlags getFlags() const noexcept;
    vk::ImageType getType() const noexcept;
    vk::Format getFormat() const noexcept;
    Extent3u const &getExtent() const noexcept;
    std::uint32_t getMipLevels() const noexcept;
    std::uint32_t getArrayLayers() const noexcept;
    vk::SampleCountFlagBits getSamples() const noexcept;
    vk::ImageTiling getTiling() const noexcept;
    vk::ImageUsageFlags getUsage() const noexcept;
    vk::SharingMode getSharingMode() const noexcept;
    vk::Image get() const noexcept;

  private:
    gsl::not_null<VmaAllocator> allocator_;
    vk::ImageCreateFlags flags_;
    vk::ImageType type_;
    vk::Format format_;
    Extent3u extent_;
    std::uint32_t mipLevels_;
    std::uint32_t arrayLayers_;
    vk::SampleCountFlagBits samples_;
    vk::ImageTiling tiling_;
    vk::ImageUsageFlags usage_;
    vk::SharingMode sharingMode_;
    vk::Image image_;
    VmaAllocation allocation_;
  };

  template<typename H>
  H AbslHashValue(H state, GpuImage const &image) noexcept {
    return H::combine(std::move(state), image.get());
  }
} // namespace imp