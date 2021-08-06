import mobula.engine.gpu;

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

namespace mobula {
  auto hash_value(GpuDescriptorSetLayoutBinding const &binding) noexcept
      -> std::size_t {
    auto seed = std::size_t{};
    boost::hash_combine(seed, binding.descriptorType);
    boost::hash_combine(seed, binding.descriptorCount);
    boost::hash_combine(
        seed, static_cast<VkShaderStageFlags>(binding.stageFlags));
    return seed;
  }
}