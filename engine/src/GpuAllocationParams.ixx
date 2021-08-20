// clang-format off
module;
#include <vk_mem_alloc.h>
export module mobula.engine.gpu:GpuAllocationParams;
// clang-format on

namespace mobula {
  /**
   * \brief Holds the parameters for a gpu allocation.
   */
  export struct GpuAllocationParams {
    VmaAllocationCreateFlags flags;
    VmaMemoryUsage usage;

    bool operator==(GpuAllocationParams const &rhs) const = default;
  };
} // namespace mobula