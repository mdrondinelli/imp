// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.gpu:RenderPass;
import :RenderPassParams;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Wrapper around a vulkan render pass.
     */
    export class RenderPass {
    public:
      /**
       * \param device the device to be used to create this render pass.
       * \param params a RenderPassParams describing this render pass.
       */
      explicit RenderPass(vk::Device device, RenderPassParams const &params);

      /**
       * \return the underlying vk::RenderPass.
       */
      vk::RenderPass getHandle() const noexcept {
        return *handle_;
      }

      /**
       * \return the RenderPassParams used to create this render pass.
       */
      RenderPassParams const &getParams() const noexcept {
        return params_;
      }

    private:
      vk::UniqueRenderPass handle_;
      RenderPassParams params_;
    };
  } // namespace gpu
} // namespace mobula