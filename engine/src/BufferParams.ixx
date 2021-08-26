// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:BufferParams;
import <vector>;
// clang-format on

namespace mobula {
  export struct BufferParams {
    vk::DeviceSize size;
    vk::BufferUsageFlags usage;
    std::vector<std::uint32_t> queueFamilyIndices;
  };
} // namespace mobula