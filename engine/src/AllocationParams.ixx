// clang-format off
module;
#include <vk_mem_alloc.h>
export module mobula.engine.vulkan:AllocationParams;
// clang-format on

namespace mobula {
  export struct AllocationParams {
    VmaAllocationCreateFlags flags;
    VmaMemoryUsage usage;
  };
}