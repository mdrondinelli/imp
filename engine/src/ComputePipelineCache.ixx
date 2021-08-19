// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:ComputePipelineCache;
import <mutex>;
import <unordered_set>;
import :ComputePipeline;
import :ComputePipelineParams;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  export class ComputePipelineCache {
  public:
    explicit ComputePipelineCache(vk::Device device);

    void clearShaderModuleCache() noexcept;

    ComputePipeline const *create(ComputePipelineParams const &params);

  private:
    struct Hash {
      using is_transparent = void;

      std::size_t
      operator()(ComputePipeline const &computePipeline) const noexcept {
        return hash_value(computePipeline.getParams());
      }

      std::size_t
      operator()(ComputePipelineParams const &params) const noexcept {
        return hash_value(params);
      }
    };

    struct Equal {
      using is_transparent = void;

      bool operator()(ComputePipeline const &lhs, ComputePipeline const &rhs)
          const noexcept {
        return &lhs == &rhs;
      }

      bool operator()(
          ComputePipeline const &lhs,
          ComputePipelineParams const &rhs) const noexcept {
        return lhs.getParams() == rhs;
      }

      bool operator()(
          ComputePipelineParams const &lhs,
          ComputePipeline const &rhs) const noexcept {
        return lhs == rhs.getParams();
      }
    };

    vk::Device device_;
    ShaderModuleCache shaderModuleCache_;
    std::unordered_set<ComputePipeline, Hash, Equal> computePipelines_;
    std::mutex mutex_;
  };
} // namespace mobula