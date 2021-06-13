#pragma once

#include "../system/GpuBuffer.h"

namespace imp {
  class Atmosphere;

  class AtmosphereBuffer {
  public:
    explicit AtmosphereBuffer(VmaAllocator allocator);

    vk::Buffer get() const noexcept;

    void update(Atmosphere const &atmosphere) noexcept;

  private:
    GpuBuffer buffer_;
    GpuBuffer createBuffer(VmaAllocator allocator);
  };
} // namespace imp