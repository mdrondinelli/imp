// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:DescriptorSetLayout;
import :DescriptorSetLayoutParams;
// clang-format on

namespace mobula {
  /**
   * A class that wraps a vk::DescriptorSetLayout.
   */
  export class DescriptorSetLayout {
  public:
    /**
     * \param device the device to be used to create this descriptor set layout.
     * \param params a DescriptorSetLayoutParams describing this descriptor set
     * layout.
     */
    explicit DescriptorSetLayout(
        vk::Device device, DescriptorSetLayoutParams const &params);

    /**
     * \return the underlying vk::DescriptorSetLayout.
     */
    vk::DescriptorSetLayout getHandle() const noexcept {
      return *handle_;
    }

    /**
     * \return the DescriptorSetLayoutParams used to create this descriptor
     * set layout.
     */
    DescriptorSetLayoutParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniqueDescriptorSetLayout handle_;
    DescriptorSetLayoutParams params_;
  };
} // namespace mobula