#include "AtmosphereBuffer.h"

#include <cstring>

#include "Atmosphere.h"

namespace imp {
  AtmosphereBuffer::AtmosphereBuffer(VmaAllocator allocator):
      buffer_{createBuffer(allocator)} {}

  vk::Buffer AtmosphereBuffer::get() const noexcept {
    return buffer_.get();
  }

  void AtmosphereBuffer::update(Atmosphere const &atmosphere) noexcept {
    auto dst = buffer_.data();
    auto push = [&](auto const &object) {
      std::memcpy(dst, &object, sizeof(object));
      dst += sizeof(object);
    };
    push(atmosphere.getRayleighScattering());
    push(atmosphere.getRayleighScaleHeight());
    push(atmosphere.getMieScattering());
    push(atmosphere.getMieAbsorption());
    push(atmosphere.getMieScaleHeight());
    push(atmosphere.getMieG());
    push(atmosphere.getOzoneAbsorption());
    push(atmosphere.getOzoneLayerHeight());
    push(atmosphere.getOzoneLayerThickness());
    push(atmosphere.getPlanetRadius());
    push(atmosphere.getAtmosphereRadius());
    buffer_.flush();
  }

  GpuBuffer AtmosphereBuffer::createBuffer(VmaAllocator allocator) {
    auto buffer = vk::BufferCreateInfo{};
    buffer.size = 60;
    buffer.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocation.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    return {allocator, buffer, allocation};
  }
} // namespace imp