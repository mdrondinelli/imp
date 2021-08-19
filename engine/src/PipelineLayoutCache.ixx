// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:PipelineLayoutCache;
import <mutex>;
import <unordered_set>;
import :PipelineLayout;
import :PipelineLayoutParams;
// clang-format on

namespace mobula {
  /**
   * A class that creates and caches pipeline layouts.
   */
  export class PipelineLayoutCache {
  public:
    /**
     * \param device the device to be used by this cache to create pipeline
     * layouts.
     */
    explicit PipelineLayoutCache(vk::Device device);

    /**
     * If this function is called with params equal to the params of a previous
     * invocation, it returns the same pipeline layout as the first invocation.
     * Otherwise, this function creates and returns a new pipeline layout.
     * \param params a description of the pipeline layout.
     * \return a pointer to the pipeline layout described by params.
     */
    PipelineLayout const *create(PipelineLayoutParams const &params);

  private:
    struct Hash {
      using is_transparent = void;

      std::size_t operator()(PipelineLayout const &layout) const noexcept {
        return hash_value(layout.getParams());
      }

      std::size_t
      operator()(PipelineLayoutParams const &params) const noexcept {
        return hash_value(params);
      }
    };

    struct Equal {
      using is_transparent = void;

      bool operator()(
          PipelineLayout const &lhs, PipelineLayout const &rhs) const noexcept {
        return &lhs == &rhs;
      }

      bool operator()(
          PipelineLayout const &lhs,
          PipelineLayoutParams const &rhs) const noexcept {
        return lhs.getParams() == rhs;
      }

      bool operator()(
          PipelineLayoutParams const &lhs,
          PipelineLayout const &rhs) const noexcept {
        return lhs == rhs.getParams();
      }
    };

    vk::Device device_;
    std::unordered_set<PipelineLayout, Hash, Equal> pipelineLayouts_;
    std::mutex mutex_;
  };
} // namespace mobula