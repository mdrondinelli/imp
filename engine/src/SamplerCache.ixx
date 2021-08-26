// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:SamplerCache;
import <mutex>;
import <unordered_set>;
import :Sampler;
import :SamplerParams;
// clang-format on

namespace mobula {
  /**
   * \brief Cache for pipeline layouts.
   */
  export class SamplerCache {
  public:
    /**
     * \param device The device to be used by this cache to create samplers.
     */
    explicit SamplerCache(vk::Device device);

    /**
     * If this function is called with params equal to the params of a previous
     * invocation, it returns the same sampler as the first invocation.
     * Otherwise this function creates and returns a new sampler.
     *
     * \param params The parameters of a sampler.
     *
     * \return A pointer to the sampler described by params.
     */
    Sampler const *get(SamplerParams const &params);

  private:
    struct Hash {
      using is_transparent = void;

      std::size_t operator()(Sampler const &sampler) const noexcept {
        return hash_value(sampler.getParams());
      }

      std::size_t operator()(SamplerParams const &params) const noexcept {
        return hash_value(params);
      }
    };

    struct Equal {
      using is_transparent = void;

      bool operator()(Sampler const &lhs, Sampler const &rhs) const noexcept {
        return &lhs == &rhs;
      }

      bool
      operator()(Sampler const &lhs, SamplerParams const &rhs) const noexcept {
        return lhs.getParams() == rhs;
      }

      bool
      operator()(SamplerParams const &lhs, Sampler const &rhs) const noexcept {
        return lhs == rhs.getParams();
      }
    };

    vk::Device device_;
    std::unordered_set<Sampler, Hash, Equal> samplers_;
    std::mutex mutex_;
  };
} // namespace mobula