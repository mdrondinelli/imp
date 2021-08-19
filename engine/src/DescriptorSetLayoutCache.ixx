// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:DescriptorSetLayoutCache;
import <mutex>;
import <unordered_set>;
import :DescriptorSetLayout;
import :DescriptorSetLayoutParams;
// clang-format on

namespace mobula {
  /**
   * A class that creates and caches descriptor set layouts.
   */
  export class DescriptorSetLayoutCache {
  public:
    /**
     * \param device a device to be used by this cache to create descriptor
     * set layouts.
     */
    explicit DescriptorSetLayoutCache(vk::Device device);

    /**
     * If this function is called with params equal to the params of a previous
     * invocation, it returns the same descriptor set layout as the first
     * invocation. Otherwise, this function creates and returns a new descriptor
     * set layout.
     *
     * \param params the parameters of a descriptor set layout.
     *
     * \return a pointer to the descriptor set layout described by params.
     */
    DescriptorSetLayout const *get(DescriptorSetLayoutParams const &params);

  private:
    struct Hash {
      using is_transparent = void;

      std::size_t operator()(
          DescriptorSetLayout const &descriptorSetLayout) const noexcept {
        return hash_value(descriptorSetLayout.getParams());
      }

      std::size_t
      operator()(DescriptorSetLayoutParams const &params) const noexcept {
        return hash_value(params);
      }
    };

    struct Equal {
      using is_transparent = void;

      bool operator()(
          DescriptorSetLayout const &lhs,
          DescriptorSetLayout const &rhs) const noexcept {
        return &lhs == &rhs;
      }

      bool operator()(
          DescriptorSetLayout const &lhs,
          DescriptorSetLayoutParams const &rhs) const noexcept {
        return lhs.getParams() == rhs;
      }

      bool operator()(
          DescriptorSetLayoutParams const &lhs,
          DescriptorSetLayout const &rhs) const noexcept {
        return lhs == rhs.getParams();
      }
    };

    vk::Device device_;
    std::unordered_set<DescriptorSetLayout, Hash, Equal> cache_;
    std::mutex mutex_;
  };
} // namespace mobula
