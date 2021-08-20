// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:GraphicsPipeline;
import :GraphicsPipelineParams;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  /**
   * \brief Wraper around a vulkan graphics pipeline.
   */
  export class GraphicsPipeline {
  public:
    /**
     * \param device The vulkan device with which this pipeline will be created.
     *
     * \param shaderModules The shader module cache to be queried during
     * construction.
     *
     * \param params The parameters of this pipeline.
     */
    explicit GraphicsPipeline(
        vk::Device device,
        ShaderModuleCache &shaderModules,
        GraphicsPipelineParams const &params);

    /**
     * \return The parameters of this pipeline.
     */
    GraphicsPipelineParams const &getParams() const noexcept {
      return params_;
    }

    /**
     * \return The underlying vulkan handle.
     */
    vk::Pipeline getHandle() const noexcept {
      return *handle_;
    }

  private:
    GraphicsPipelineParams params_;
    vk::UniquePipeline handle_;
  };
} // namespace mobula