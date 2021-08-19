// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:DescriptorSetLayoutParams;
import <vector>;
// clang-format on

namespace mobula {
  export struct DescriptorSetLayoutParams {
    struct Binding {
      vk::DescriptorType descriptorType;
      std::uint32_t descriptorCount;
      vk::ShaderStageFlags stageFlags;

      friend bool operator==(Binding const &lhs, Binding const &rhs) = default;
    };

    vk::DescriptorSetLayoutCreateFlags flags;
    std::vector<Binding> bindings;

    friend constexpr bool operator==(
        DescriptorSetLayoutParams const &lhs,
        DescriptorSetLayoutParams const &rhs) = default;
  };

  export std::size_t
  hash_value(DescriptorSetLayoutParams::Binding const &binding) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, binding.descriptorType);
    hash_combine(seed, binding.descriptorCount);
    hash_combine(seed, static_cast<VkFlags>(binding.stageFlags));
    return seed;
  }

  export std::size_t
  hash_value(DescriptorSetLayoutParams const &params) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, static_cast<VkFlags>(params.flags));
    boost::hash_range(seed, begin(params.bindings), end(params.bindings));
    return seed;
  }
} // namespace mobula