// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
export module mobula.gpu:Allocator;
import :AllocationParams;
import :Buffer;
import :BufferParams;
import :Image;
import :ImageParams;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Wrapper around VmaAllocator.
     */
    export class Allocator {
    public:
      explicit Allocator(
          vk::PhysicalDevice physicalDevice,
          vk::Device device,
          vk::Instance instance);

      Allocator(Allocator &&rhs) noexcept;

      Allocator &operator=(Allocator &&rhs) noexcept;

      ~Allocator();

      /**
       * \param bufferParams The parameters of the new buffer.
       *
       * \param allocationParams The parameters of the new buffer's allocation.
       *
       * \return A new buffer according to bufferParams and allocationParams.
       */
      Buffer create(
          BufferParams const &bufferParams,
          AllocationParams const &allocationParams);

      /**
       * \param imageParams The parameters of the new image.
       *
       * \param allocationParams The parameters of the new image's allocation.
       * 
       * \return A new image according to imageParams and allocationParams.
       */
      Image create(
          ImageParams const &imageParams,
          AllocationParams const &allocationParams);

    private:
      VmaAllocator allocator_;
    };
  } // namespace gpu
} // namespace mobula