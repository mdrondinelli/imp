import mobula.engine.gpu;

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

namespace mobula {
  auto hash_value(GpuSubpassDependency const& dep) noexcept -> std::size_t {
    auto seed = std::size_t{};
    boost::hash_combine(seed, dep.srcSubpass);
    boost::hash_combine(seed, dep.dstSubpass);
    boost::hash_combine(seed, static_cast<VkFlags>(dep.srcStageMask));
    boost::hash_combine(seed, static_cast<VkFlags>(dep.dstStageMask));
    return seed;
  }
}