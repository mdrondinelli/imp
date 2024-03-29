// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.gpu:ComputePipelineParams;
import :PipelineLayout;
import :PipelineShaderStageParams;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Holds the parameters of a compute pipeline.
     */
    export struct ComputePipelineParams {
      vk::PipelineCreateFlags flags;
      PipelineLayout const *layout;
      PipelineShaderStageParams computeStage;

      bool operator==(ComputePipelineParams const &rhs) const = default;
    };

    export std::size_t
    hash_value(ComputePipelineParams const &params) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, static_cast<VkFlags>(params.flags));
      hash_combine(seed, params.layout);
      hash_combine(seed, params.computeStage);
      return seed;
    }
  } // namespace gpu
} // namespace mobula