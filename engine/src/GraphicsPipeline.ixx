// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:GraphicsPipeline;
import :GraphicsPipelineParams;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  export class GraphicsPipeline {
  public:
    explicit GraphicsPipeline(
        vk::Device device,
        ShaderModuleCache &shaderModuleCache,
        GraphicsPipelineParams const &params);

    vk::Pipeline getHandle() const noexcept {
      return *handle_;
    }

    GraphicsPipelineParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniquePipeline handle_;
    GraphicsPipelineParams params_;
  };
} // namespace mobula