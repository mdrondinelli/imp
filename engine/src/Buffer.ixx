// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
export module mobula.gpu:Buffer;
import :AllocationParams;
import :BufferParams;
import :MappedMemory;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Wrapper around vulkan buffer and bound memory.
     */
    export class Buffer {
    public:
      /**
       * \param allocator The allocator with which this buffer will be
       * allocated.
       *
       * \param bufferParams The parameters of this buffer.
       *
       * \param allocationParams The parameters of this buffer's allocation.
       */
      explicit Buffer(
          VmaAllocator allocator,
          BufferParams const &bufferParams,
          AllocationParams const &allocationParams);

      Buffer(Buffer &&rhs) noexcept;

      Buffer &operator=(Buffer &&rhs) noexcept;

      ~Buffer();

      /**
       * \return The parameters of this buffer.
       */
      BufferParams const &getBufferParams() const noexcept {
        return bufferParams_;
      }

      /**
       * \return The parameters of this buffer's allocation.
       */
      AllocationParams const &getAllocationParams() const noexcept {
        return allocationParams_;
      }

      /**
       * \return The underlying buffer handle.
       */
      vk::Buffer getBuffer() const noexcept {
        return buffer_;
      }

      /**
       * \return The underlying allocation pointer.
       */
      VmaAllocation getAllocation() const noexcept {
        return allocation_;
      }

      /**
       * \return A MappedMemory object through which this buffer's memory can be
       * accessed.
       */
      MappedMemory map();

    private:
      VmaAllocator allocator_;
      BufferParams bufferParams_;
      AllocationParams allocationParams_;
      vk::Buffer buffer_;
      VmaAllocation allocation_;
    };
  } // namespace gpu
} // namespace mobula