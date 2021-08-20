// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:GpuBufferParams;
import <vector>;
import :GpuAllocationParams;
// clang-format on

namespace mobula {
  /**
   * \brief Holds the parameters for a gpu buffer.
   */
  export struct GpuBufferParams {
    vk::BufferCreateFlags flags;
    vk::BufferUsageFlags usage;
    vk::DeviceSize size;
    std::vector<std::uint32_t> queueFamilyIndices;
    GpuAllocationParams allocation;

    bool operator==(GpuBufferParams const &rhs) const = default;
  };
} // namespace mobula