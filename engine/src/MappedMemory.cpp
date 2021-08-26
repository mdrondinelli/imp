// clang-format off
module;
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
module mobula.engine.vulkan;
import <cstddef>;
import <stdexcept>;
// clang-format on

namespace mobula {
  MappedMemory::MappedMemory(VmaAllocator allocator, VmaAllocation allocation):
      allocator_{allocator}, allocation_{allocation} {
    void *data;
    if (vmaMapMemory(allocator_, allocation_, &data)) {
      throw std::runtime_error{"Failed to map memory."};
    }
    data_ = static_cast<std::byte *>(data);
  }

  MappedMemory::~MappedMemory() {
    if (allocator_) {
      vmaUnmapMemory(allocator_, allocation_);
    }
  }

  MappedMemory::MappedMemory(MappedMemory &&rhs) noexcept:
      allocator_{rhs.allocator_},
      allocation_{rhs.allocation_},
      data_{rhs.data_} {
    rhs.allocator_ = nullptr;
    rhs.allocation_ = nullptr;
    rhs.data_ = nullptr;
  }

  MappedMemory &MappedMemory::operator=(MappedMemory &&rhs) noexcept {
    if (&rhs != this) {
      allocator_ = rhs.allocator_;
      allocation_ = rhs.allocation_;
      data_ = rhs.data_;
      rhs.allocator_ = nullptr;
      rhs.allocation_ = nullptr;
      rhs.data_ = nullptr;
    }
    return *this;
  }

  void MappedMemory::flush() noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    vmaFlushAllocation(allocator_, allocation_, 0, info.size);
  }

  void
  MappedMemory::flush(vk::DeviceSize offset, vk::DeviceSize size) noexcept {
    vmaFlushAllocation(allocator_, allocation_, offset, size);
  }

  void MappedMemory::invalidate() noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    vmaInvalidateAllocation(allocator_, allocation_, 0, info.size);
  }

  void MappedMemory::invalidate(
      vk::DeviceSize offset, vk::DeviceSize size) noexcept {
    vmaInvalidateAllocation(allocator_, allocation_, offset, size);
  }
} // namespace mobula