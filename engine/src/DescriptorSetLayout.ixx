// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:DescriptorSetLayout;
import :DescriptorSetLayoutParams;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around a vulkan descriptor set layout.
   */
  export class DescriptorSetLayout {
  public:
    /**
     * \param device a vulkan device to be used to create this descriptor set
     * layout.
     *
     * \param params a struct describing the parameters of this descriptor set
     * layout.
     */
    explicit DescriptorSetLayout(
        vk::Device device, DescriptorSetLayoutParams const &params);

    /**
     * \return the underlying vulkan handle.
     */
    vk::DescriptorSetLayout getHandle() const noexcept {
      return *handle_;
    }

    /**
     * \return the parameters of this descriptor set layout.
     */
    DescriptorSetLayoutParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniqueDescriptorSetLayout handle_;
    DescriptorSetLayoutParams params_;
  };
} // namespace mobula