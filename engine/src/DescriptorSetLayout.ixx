// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:DescriptorSetLayout;
import :DescriptorSetLayoutParams;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around a vulkan descriptor set layout.
   */
  export class DescriptorSetLayout {
  public:
    /**
     * \param device The vulkan device with which this descriptor set layout
     * will be created.
     *
     * \param params The parameters of this descriptor set layout.
     */
    explicit DescriptorSetLayout(
        vk::Device device, DescriptorSetLayoutParams const &params);

    /**
     * \return the parameters of this descriptor set layout.
     */
    DescriptorSetLayoutParams const &getParams() const noexcept {
      return params_;
    }

    /**
     * \return the underlying vulkan handle.
     */
    vk::DescriptorSetLayout getHandle() const noexcept {
      return *handle_;
    }

  private:
    DescriptorSetLayoutParams params_;
    vk::UniqueDescriptorSetLayout handle_;
  };
} // namespace mobula