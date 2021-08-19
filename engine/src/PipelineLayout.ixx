// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:PipelineLayout;
import :PipelineLayoutParams;
// clang-format on

namespace mobula {
  /**
   * A class that wraps a vk::PipelineLayout.
   */
  export class PipelineLayout {
  public:
    /**
     * \param device the device to be used to create this pipeline layout.
     * \param info a PipelineLayoutParams describing this pipeline layout.
     */
    explicit PipelineLayout(
        vk::Device device, PipelineLayoutParams const &params);

    /**
     * \return the underlying vk::PipelineLayout.
     */
    vk::PipelineLayout getHandle() const noexcept {
      return *handle_;
    }

    /**
     * \return the PipelineLayoutParams used to create this pipeline layout.
     */
    PipelineLayoutParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniquePipelineLayout handle_;
    PipelineLayoutParams params_;
  };
} // namespace mobula