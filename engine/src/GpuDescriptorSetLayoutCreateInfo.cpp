import mobula.engine.gpu;

// clang-format off
import <algorithm>;
// clang-format on

#include <boost/container_hash/hash.hpp>

namespace mobula {
  auto operator==(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept -> bool {
    return lhs.bindings.data() == rhs.bindings.data() &&
               lhs.bindings.size() == rhs.bindings.size() ||
           std::ranges::equal(lhs.bindings, rhs.bindings);
  }

  auto operator!=(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  auto hash_value(GpuDescriptorSetLayoutCreateInfo const &createInfo) noexcept
      -> std::size_t {
    return boost::hash_range(
        createInfo.bindings.begin(), createInfo.bindings.end());
  }
} // namespace mobula