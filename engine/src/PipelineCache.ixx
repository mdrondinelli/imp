// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:PipelineCache;
import <mutex>;
import <unordered_set>;
import :ComputePipeline;
import :ComputePipelineParams;
import :GraphicsPipeline;
import :GraphicsPipelineParams;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  /**
   * \brief Cache for graphics and compute pipelines.
   */
  export class PipelineCache {
  public:
    /**
     * \param device the device to be used by this cache to create pipelines
     */
    explicit PipelineCache(vk::Device device);

    /**
     * If this function is called with params equal to the params of a previous
     * invocation, it returns the same pipeline as the first invocation.
     * Otherwise, this function creates and returns a new pipeline.
     *
     * \param params the parameters of a compute pipeline.
     *
     * \return a pointer to the compute pipeline described by params.
     */
    ComputePipeline const *get(ComputePipelineParams const &params);

    /**
     * If this function is called with params equal to the params of a previous
     * invocation, it returns the same pipeline as the first invocation.
     * Otherwise, this function creates and returns a new pipeline.
     *
     * \param params a description of a graphics pipeline.
     *
     * \return a pointer to the compute pipeline described by params.
     */
    GraphicsPipeline const *get(GraphicsPipelineParams const &params);

    /**
     * Clears the internal shader module cache.
     * \sa ShaderModuleCache::clear
     */
    void clearShaderModules() noexcept;

  private:
    struct ComputePipelineHash {
      using is_transparent = void;

      std::size_t
      operator()(ComputePipeline const &computePipeline) const noexcept {
        return hash_value(computePipeline.getParams());
      }

      std::size_t operator()(
          ComputePipelineParams const &computePipelineParams) const noexcept {
        return hash_value(computePipelineParams);
      }
    };

    struct ComputePipelineEqual {
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

    struct GraphicsPipelineHash {
      using is_transparent = void;

      std::size_t
      operator()(GraphicsPipeline const &graphicsPipeline) const noexcept {
        return hash_value(graphicsPipeline.getParams());
      }

      std::size_t operator()(
          GraphicsPipelineParams const &graphicsPipelineParams) const noexcept {
        return hash_value(graphicsPipelineParams);
      }
    };

    struct GraphicsPipelineEqual {
      using is_transparent = void;

      bool operator()(GraphicsPipeline const &lhs, GraphicsPipeline const &rhs)
          const noexcept {
        return &lhs == &rhs;
      }

      bool operator()(
          GraphicsPipeline const &lhs,
          GraphicsPipelineParams const &rhs) const noexcept {
        return lhs.getParams() == rhs;
      }

      bool operator()(
          GraphicsPipelineParams const &lhs,
          GraphicsPipeline const &rhs) const noexcept {
        return lhs == rhs.getParams();
      }
    };

    vk::Device device_;
    ShaderModuleCache shaderModules_;
    std::unordered_set<
        ComputePipeline,
        ComputePipelineHash,
        ComputePipelineEqual>
        computePipelines_;
    std::unordered_set<
        GraphicsPipeline,
        GraphicsPipelineHash,
        GraphicsPipelineEqual>
        graphicsPipelines_;
    std::mutex computePipelinesMutex_;
    std::mutex graphicsPipelinesMutex_;
  };
} // namespace mobula