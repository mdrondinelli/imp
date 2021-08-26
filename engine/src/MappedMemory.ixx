// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:MappedMemory;
import <cstddef>;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around mapped vulkan memory.
   */
  export class MappedMemory {
  public:
    explicit MappedMemory(VmaAllocator allocator, VmaAllocation allocation);
    ~MappedMemory();

    MappedMemory(MappedMemory &&rhs) noexcept;
    MappedMemory &operator=(MappedMemory &&rhs) noexcept;

    /**
     * \return A pointer to the underlying mapped memory.
     */
    std::byte const *data() const noexcept {
      return data_;
    }

    /**
     * \sa MappedMemory::data
     */
    std::byte *data() noexcept {
      return data_;
    }

    /**
     * \brief Flushes the entirety of mapped memory.
     */
    void flush() noexcept;

    /**
     * \brief Flushes a subregion of mapped memory.
     */
    void flush(vk::DeviceSize offset, vk::DeviceSize size) noexcept;

    /**
     * \brief Invalidates the entirety of mapped memory.
     */
    void invalidate() noexcept;

    /**
     * \brief Invalidates a subregion of mapped memory.
     */
    void invalidate(vk::DeviceSize offset, vk::DeviceSize size) noexcept;

  private:
    VmaAllocator allocator_;
    VmaAllocation allocation_;
    std::byte *data_;
  };
} // namespace mobula