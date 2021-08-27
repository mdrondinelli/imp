// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.gpu:ImageParams;
import mobula.util;
// clang-format on

namespace mobula {
  namespace gpu {
    export struct ImageParams {
      vk::ImageType type;
      vk::Format format;
      Extent3u extent;
      std::uint32_t mipLevels;
      std::uint32_t arrayLayers;
      vk::ImageTiling tiling;
      vk::ImageUsageFlags usage;
      std::vector<std::uint32_t> queueFamilyIndices;
    };
  } // namespace gpu
} // namespace mobula