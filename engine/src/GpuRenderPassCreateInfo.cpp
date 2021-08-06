import mobula.engine.gpu;

#include <boost/container_hash/hash.hpp>

// clang-format off
import <algorithm>;
// clang-format on

namespace mobula {
  auto operator==(
      GpuRenderPassCreateInfo const &lhs,
      GpuRenderPassCreateInfo const &rhs) noexcept -> bool {
    return (lhs.attachments.data() == rhs.attachments.data() &&
                lhs.attachments.size() == rhs.attachments.size() ||
            std::ranges::equal(lhs.attachments, rhs.attachments)) &&
           (lhs.subpasses.data() == rhs.subpasses.data() &&
                lhs.subpasses.size() == rhs.subpasses.size() ||
            std::ranges::equal(lhs.attachments, rhs.attachments)) &&
           (lhs.dependencies.data() == rhs.dependencies.data() &&
                lhs.dependencies.size() == rhs.dependencies.size() ||
            std::ranges::equal(lhs.dependencies, rhs.dependencies));
  }

  auto operator!=(
      GpuRenderPassCreateInfo const &lhs,
      GpuRenderPassCreateInfo const &rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  auto hash_value(GpuRenderPassCreateInfo const &createInfo) noexcept
      -> std::size_t {}
} // namespace mobula