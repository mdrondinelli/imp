// clang-format off
module;
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
export module mobula.engine.gpu:GpuBuffer;
import <cstddef>;
import :GpuBufferParams;
import :MappedMemory;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around vulkan buffer and memory.
   */
  export class GpuBuffer {
  public:
    /**
     * \param allocator The allocator from which this buffer will be allocated.
     *
     * \param params The parameters of this buffer.
     */
    explicit GpuBuffer(VmaAllocator allocator, GpuBufferParams const &params);

    ~GpuBuffer();

    GpuBuffer(GpuBuffer &&rhs) noexcept;

    GpuBuffer &operator=(GpuBuffer &&rhs) noexcept;

    /**
     * \return The parameters of this buffer.
     */
    GpuBufferParams const &getParams() const noexcept {
      return params_;
    }

    /**
     * \return The underlying vulkan handle.
     */
    vk::Buffer getHandle() const noexcept {
      return buffer_;
    }

    /**
     * \brief Maps this buffer's memory to make it accessible to the CPU.
     * \return A \c MappedMemory object representing the mapped memory.
     */
    MappedMemory map() noexcept;

  private:
    VmaAllocator allocator_;
    GpuBufferParams params_;
    vk::Buffer buffer_;
    VmaAllocation allocation_;
  };
} // namespace mobula