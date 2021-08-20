#include "GpuBuffer.h"

#include <sstream>
#include <stdexcept>

#include "GpuBufferError.h"

namespace imp {
  GpuBuffer::GpuBuffer(
      gsl::not_null<VmaAllocator> allocator,
      vk::BufferCreateInfo const &bufferCreateInfo,
      VmaAllocationCreateInfo const &allocationCreateInfo,
      std::string_view name):
      allocator_{allocator},
      flags_{bufferCreateInfo.flags},
      size_{bufferCreateInfo.size},
      usage_{bufferCreateInfo.usage},
      sharingMode_{bufferCreateInfo.sharingMode} {
    if (auto result = vmaCreateBuffer(
            allocator_,
            reinterpret_cast<VkBufferCreateInfo const *>(&bufferCreateInfo),
            &allocationCreateInfo,
            reinterpret_cast<VkBuffer *>(&buffer_),
            &allocation_,
            nullptr);
        result != VK_SUCCESS) {
      auto oss = std::ostringstream{};
      oss << "failed to create " << name << " (code " << result << ")\n";
      throw GpuBufferError{oss.str()};
    }
  }

  GpuBuffer::~GpuBuffer() {
    vmaDestroyBuffer(allocator_, buffer_, allocation_);
  }

  GpuBuffer::GpuBuffer(GpuBuffer &&rhs) noexcept:
      allocator_{rhs.allocator_},
      flags_{rhs.flags_},
      size_{rhs.size_},
      usage_{rhs.usage_},
      buffer_{rhs.buffer_},
      allocation_{rhs.allocation_} {
    rhs.buffer_ = nullptr;
    rhs.allocation_ = nullptr;
  }

  GpuBuffer &GpuBuffer::operator=(GpuBuffer &&rhs) noexcept {
    if (&rhs != this) {
      vmaDestroyBuffer(allocator_, buffer_, allocation_);
      allocator_ = rhs.allocator_;
      flags_ = rhs.flags_;
      size_ = rhs.size_;
      usage_ = rhs.usage_;
      buffer_ = rhs.buffer_;
      allocation_ = rhs.allocation_;
      rhs.buffer_ = nullptr;
      rhs.allocation_ = nullptr;
    }
    return *this;
  }

  gsl::not_null<char *> GpuBuffer::map() {
    void *data;
    if (vmaMapMemory(allocator_, allocation_, &data)) {
      throw std::runtime_error{"failed to map gpu buffer memory."};
    }
    return gsl::not_null{static_cast<char *>(data)};
  }

  void GpuBuffer::unmap() noexcept {
    vmaUnmapMemory(allocator_, allocation_);
  }

  void GpuBuffer::flush() noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    vmaFlushAllocation(allocator_, allocation_, 0, info.size);
  }

  void GpuBuffer::flush(vk::DeviceSize offset, vk::DeviceSize size) noexcept {
    vmaFlushAllocation(allocator_, allocation_, offset, size);
  }

  void GpuBuffer::invalidate() noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    vmaInvalidateAllocation(allocator_, allocation_, 0, info.size);
  }

  void
  GpuBuffer::invalidate(vk::DeviceSize offset, vk::DeviceSize size) noexcept {
    vmaInvalidateAllocation(allocator_, allocation_, offset, size);
  }

  char *GpuBuffer::getMappedData() const noexcept {
    auto info = VmaAllocationInfo{};
    vmaGetAllocationInfo(allocator_, allocation_, &info);
    return static_cast<char *>(info.pMappedData);
  }

  vk::BufferCreateFlags GpuBuffer::getFlags() const noexcept {
    return flags_;
  }

  vk::DeviceSize GpuBuffer::getSize() const noexcept {
    return size_;
  }

  vk::BufferUsageFlags GpuBuffer::getUsage() const noexcept {
    return usage_;
  }

  vk::SharingMode GpuBuffer::getSharingMode() const noexcept {
    return sharingMode_;
  }

  vk::Buffer GpuBuffer::get() const noexcept {
    return buffer_;
  }
} // namespace imp