// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
module mobula.gpu;
import <stdexcept>;
// clang-format on

namespace mobula {
  namespace gpu {
    Image::Image(
        VmaAllocator allocator,
        ImageParams const &imageParams,
        AllocationParams const &allocationParams):
        allocator_{allocator},
        imageParams_{imageParams},
        allocationParams_{allocationParams} {
      auto imageCreateInfo = vk::ImageCreateInfo{};
      imageCreateInfo.imageType = imageParams.type;
      imageCreateInfo.format = imageParams.format;
      imageCreateInfo.extent.width = imageParams.extent.width;
      imageCreateInfo.extent.height = imageParams.extent.height;
      imageCreateInfo.extent.depth = imageParams.extent.depth;
      imageCreateInfo.mipLevels = imageParams.mipLevels;
      imageCreateInfo.arrayLayers = imageParams.arrayLayers;
      imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
      imageCreateInfo.tiling = imageParams.tiling;
      imageCreateInfo.usage = imageParams.usage;
      if (!imageParams.queueFamilyIndices.empty()) {
        imageCreateInfo.sharingMode = vk::SharingMode::eConcurrent;
        imageCreateInfo.queueFamilyIndexCount =
            static_cast<std::uint32_t>(imageParams.queueFamilyIndices.size());
        imageCreateInfo.pQueueFamilyIndices =
            imageParams.queueFamilyIndices.data();
      }
      auto allocationCreateInfo = VmaAllocationCreateInfo{};
      allocationCreateInfo.flags = allocationParams.flags;
      allocationCreateInfo.usage = allocationParams.usage;
      if (vmaCreateImage(
              allocator_,
              reinterpret_cast<VkImageCreateInfo *>(&imageCreateInfo),
              &allocationCreateInfo,
              reinterpret_cast<VkImage *>(&image_),
              &allocation_,
              nullptr) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create image."};
      }
    }

    Image::Image(Image &&rhs) noexcept:
        allocator_{rhs.allocator_},
        imageParams_{std::move(rhs.imageParams_)},
        allocationParams_{rhs.allocationParams_},
        image_{rhs.image_},
        allocation_{rhs.allocation_} {
      rhs.image_ = vk::Image{};
      rhs.allocation_ = nullptr;
    }

    Image &Image::operator=(Image &&rhs) noexcept {
      if (&rhs != this) {
        allocator_ = rhs.allocator_;
        imageParams_ = std::move(rhs.imageParams_);
        allocationParams_ = rhs.allocationParams_;
        image_ = rhs.image_;
        allocation_ = rhs.allocation_;
        rhs.image_ = vk::Image{};
        rhs.allocation_ = nullptr;
      }
      return *this;
    }

    Image::~Image() {
      vmaDestroyImage(allocator_, image_, allocation_);
    }
  } // namespace gpu
} // namespace mobula