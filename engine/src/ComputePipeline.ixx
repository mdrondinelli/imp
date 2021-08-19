// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:ComputePipeline;
import :ComputePipelineParams;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  export class ComputePipeline {
  public:
    explicit ComputePipeline(
        vk::Device device,
        ShaderModuleCache &shaderModuleCache,
        ComputePipelineParams const &params);

    vk::Pipeline getHandle() const noexcept {
      return *handle_;
    }

    ComputePipelineParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniquePipeline handle_;
    ComputePipelineParams params_;
  };
} // namespace mobula