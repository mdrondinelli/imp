// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.gpu:BufferParams;
import <vector>;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Holds the parameters of a buffer.
     */
    export struct BufferParams {
      vk::DeviceSize size;
      vk::BufferUsageFlags usage;
      std::vector<std::uint32_t> queueFamilyIndices;
    };
  } // namespace gpu
} // namespace mobula