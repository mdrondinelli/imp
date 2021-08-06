import mobula.engine.gpu;

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

namespace mobula {
  auto hash_value(GpuAttachmentDescription const &desc) noexcept
      -> std::size_t {
    auto seed = std::size_t{};
    boost::hash_combine(seed, static_cast<VkFlags>(desc.flags));
    boost::hash_combine(seed, desc.format);
    boost::hash_combine(seed, desc.samples);
    boost::hash_combine(seed, desc.loadOp);
    boost::hash_combine(seed, desc.storeOp);
    boost::hash_combine(seed, desc.stencilLoadOp);
    boost::hash_combine(seed, desc.stencilStoreOp);
    boost::hash_combine(seed, desc.initialLayout);
    boost::hash_combine(seed, desc.finalLayout);
    return seed;
  }
} // namespace mobula