// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:ComputePipeline;
import :ComputePipelineParams;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around a vulkan compute pipeline.
   */
  export class ComputePipeline {
  public:
    /**
     * \param device The vulkan device with which this pipeline will be created.
     *
     * \param shaderModules The shader module cache to be queried during
     * construction.
     *
     * \param params The parameters of this pipeline.
     */
    explicit ComputePipeline(
        vk::Device device,
        ShaderModuleCache &shaderModules,
        ComputePipelineParams const &params);

    /**
     * \return The parameters of this pipeline.
     */
    ComputePipelineParams const &getParams() const noexcept {
      return params_;
    }

    /**
     * \return The underlying vulkan handle.
     */
    vk::Pipeline getHandle() const noexcept {
      return *handle_;
    }

  private:
    ComputePipelineParams params_;
    vk::UniquePipeline handle_;
  };
} // namespace mobula