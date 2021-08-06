import mobula.engine.gpu;

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

namespace mobula {
  auto hash_value(GpuAttachmentReference const& ref) noexcept -> std::size_t {
    auto seed = std::size_t{};
    boost::hash_combine(seed, ref.attachment);
    boost::hash_combine(seed, ref.layout);
    return seed;
  }
}