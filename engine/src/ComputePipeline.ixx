// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:ComputePipeline;
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
     * \param device a vulkan device to be used to create this pipeline.
     *
     * \param shaderModules a shader module cache to be queries while creating
     * this pipeline.
     *
     * \param params a struct describing the parameters of this pipeline.
     */
    explicit ComputePipeline(
        vk::Device device,
        ShaderModuleCache &shaderModules,
        ComputePipelineParams const &params);

    /**
     * \return the underlying vulkan handle.
     */
    vk::Pipeline getHandle() const noexcept {
      return *handle_;
    }

    /**
     * \return the parameters of this pipeline.
     */
    ComputePipelineParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniquePipeline handle_;
    ComputePipelineParams params_;
  };
} // namespace mobula