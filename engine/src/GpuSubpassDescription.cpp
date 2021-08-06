import mobula.engine.gpu;

// clang-format off
import<algorithm>;
// clang-format on

#include <boost/container_hash/hash.hpp>

namespace mobula {
  auto operator==(
      GpuSubpassDescription const &lhs,
      GpuSubpassDescription const &rhs) noexcept -> bool {
    return lhs.pipelineBindPoint == rhs.pipelineBindPoint &&
           (lhs.inputAttachments.data() == rhs.inputAttachments.data() &&
                lhs.inputAttachments.size() == rhs.inputAttachments.size() ||
            std::ranges::equal(lhs.inputAttachments, rhs.inputAttachments)) &&
           (lhs.colorAttachments.data() == rhs.colorAttachments.data() &&
                lhs.colorAttachments.size() == rhs.colorAttachments.size() ||
            std::ranges::equal(lhs.colorAttachments, rhs.colorAttachments)) &&
           (lhs.resolveAttachments.data() == rhs.resolveAttachments.data() &&
                lhs.resolveAttachments.size() ==
                    rhs.resolveAttachments.size() ||
            std::ranges::equal(
                lhs.resolveAttachments, rhs.resolveAttachments)) &&
           (lhs.depthStencilAttachment == rhs.depthStencilAttachment ||
            *lhs.depthStencilAttachment == *rhs.depthStencilAttachment) &&
           (lhs.preserveAttachments.data() == rhs.preserveAttachments.data() &&
                lhs.preserveAttachments.size() ==
                    rhs.preserveAttachments.size() ||
            std::ranges::equal(
                lhs.preserveAttachments, rhs.preserveAttachments));
  }

  auto operator!=(
      GpuSubpassDescription const &lhs,
      GpuSubpassDescription const &rhs) noexcept -> bool {
    return !(lhs == rhs);
  }

  auto hash_value(GpuSubpassDescription const& desc) noexcept -> std::size_t {
    auto seed = std::size_t{};
    boost::hash_combine(seed, desc.pipelineBindPoint);
    boost::hash_range(
        seed, desc.inputAttachments.begin(), desc.inputAttachments.end());
    boost::hash_range(
        seed, desc.colorAttachments.begin(), desc.colorAttachments.end());
    boost::hash_range(
        seed, desc.resolveAttachments.begin(), desc.resolveAttachments.end());
    if (desc.depthStencilAttachment) {
      boost::hash_combine(seed, *desc.depthStencilAttachment);
    }
    boost::hash_range(
        seed, desc.preserveAttachments.begin(), desc.preserveAttachments.end());
    return seed;
  }
} // namespace mobula