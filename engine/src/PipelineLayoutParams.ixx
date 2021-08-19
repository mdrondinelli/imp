// clang-format off
module;
#include <vulkan/vulkan.hpp>
#include <boost/container_hash/hash.hpp>
export module mobula.engine.gpu:PipelineLayoutParams;
import <vector>;
import :DescriptorSetLayout;
// clang-format on

namespace mobula {
  export struct PipelineLayoutParams {
    struct PushConstantRange {
      vk::ShaderStageFlags stageFlags;
      std::uint32_t offset;
      std::uint32_t size;

      bool operator==(PushConstantRange const &rhs) const = default;
    };

    std::vector<DescriptorSetLayout const *> setLayouts;
    std::vector<PushConstantRange> pushConstantRanges;

    bool operator==(PipelineLayoutParams const &rhs) const = default;
  };

  export std::size_t hash_value(PipelineLayoutParams::PushConstantRange const
                                    &pushConstantRange) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, static_cast<VkFlags>(pushConstantRange.stageFlags));
    hash_combine(seed, pushConstantRange.offset);
    hash_combine(seed, pushConstantRange.size);
    return seed;
  }

  export std::size_t hash_value(PipelineLayoutParams const &params) noexcept {
    using boost::hash_range;
    auto seed = std::size_t{};
    hash_range(seed, begin(params.setLayouts), end(params.setLayouts));
    hash_range(
        seed, begin(params.pushConstantRanges), end(params.pushConstantRanges));
    return seed;
  }
} // namespace mobula