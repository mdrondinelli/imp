// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:VulkanMemoryParams;
// clang-format on

namespace mobula {
  export struct VulkanAllocationParams {
    VmaAllocationCreateFlags flags;
    VmaMemoryUsage usage;
    vk::MemoryPropertyFlags requiredFlags;
    vk::MemoryPropertyFlags preferredFlags;
  };
}