// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:GraphicsPipeline;
import :GraphicsPipelineParams;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  /**
   * A class that wraps a vulkan graphics pipeline.
   */
  export class GraphicsPipeline {
  public:
    /**
     * \param device a vulkan device to be used to create this pipeline.
     *
     * \param shaderModules a shader module cache to be queried while creating
     * this pipeline.
     *
     * \param params a struct describing the parameters of this pipeline.
     */
    explicit GraphicsPipeline(
        vk::Device device,
        ShaderModuleCache &shaderModules,
        GraphicsPipelineParams const &params);

    /**
     * \return the underlying vulkan handle.
     */
    vk::Pipeline getHandle() const noexcept {
      return *handle_;
    }

    /**
     * \return the parameters of this pipeline.
     */
    GraphicsPipelineParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniquePipeline handle_;
    GraphicsPipelineParams params_;
  };
} // namespace mobula