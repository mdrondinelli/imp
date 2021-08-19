// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:ComputePipelineParams;
import :PipelineLayout;
import :PipelineShaderStageState;
// clang-format on

namespace mobula {
  export struct ComputePipelineParams {
    vk::PipelineCreateFlags flags;
    PipelineLayout const *layout;
    PipelineShaderStageState computeStageState;

    bool operator==(ComputePipelineParams const &rhs) const = default;
  };

  export std::size_t hash_value(ComputePipelineParams const &params) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, static_cast<VkFlags>(params.flags));
    hash_combine(seed, params.layout);
    hash_combine(seed, params.computeStageState);
    return seed;
  }
} // namespace mobula