// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:PipelineLayout;
import :PipelineLayoutParams;
// clang-format on

namespace mobula {
  /**
   *\brief Wrapper around a vulkan pipeline layout.
   */
  export class PipelineLayout {
  public:
    /**
     * \param device a device to be used to create this pipeline layout.
     * \param params a struct describing the parameters of this pipeline layout.
     */
    explicit PipelineLayout(
        vk::Device device, PipelineLayoutParams const &params);

    /**
     * \return the underlying vulkan handle.
     */
    vk::PipelineLayout getHandle() const noexcept {
      return *handle_;
    }

    /**
     * \return the parameters of this pipeline layout.
     */
    PipelineLayoutParams const &getParams() const noexcept {
      return params_;
    }

  private:
    vk::UniquePipelineLayout handle_;
    PipelineLayoutParams params_;
  };
} // namespace mobula