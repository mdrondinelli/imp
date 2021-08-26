// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:Image;
import :AllocationParams;
import :ImageParams;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around vulkan image and bound memory.
   */
  export class Image {
  public:
    /**
     * \param allocator The allocator with which this image will be allocated.
     *
     * \param imageParams The parameters of this image.
     *
     * \param allocatorParams The parameters of this image's allocation.
     */
    explicit Image(
        VmaAllocator allocator,
        ImageParams const &imageParams,
        AllocationParams const &allocationParams);

    Image(Image &&rhs) noexcept;

    Image &operator=(Image &&rhs) noexcept;

    ~Image();

    /**
     * \return The parameters of this image.
     */
    ImageParams const &getImageParams() const noexcept {
      return imageParams_;
    }

    /**
     * \return The parameters of this image's allocation.
     */
    AllocationParams const &getAllocationParams() const noexcept {
      return allocationParams_;
    }

    /**
     * \return The underlying image handle.
     */
    vk::Image getImage() const noexcept {
      return image_;
    }

    /**
     * \return The underlying allocation pointer.
     */
    VmaAllocation getAllocation() const noexcept {
      return allocation_;
    }

  private:
    VmaAllocator allocator_;
    ImageParams imageParams_;
    AllocationParams allocationParams_;
    vk::Image image_;
    VmaAllocation allocation_;
  };
} // namespace mobula