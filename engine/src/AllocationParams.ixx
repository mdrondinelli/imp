// clang-format off
module;
#include <vk_mem_alloc.h>
export module mobula.gpu:AllocationParams;
// clang-format on

namespace mobula {
  namespace gpu {
    export struct AllocationParams {
      VmaAllocationCreateFlags flags;
      VmaMemoryUsage usage;
    };
  } // namespace gpu
} // namespace mobula