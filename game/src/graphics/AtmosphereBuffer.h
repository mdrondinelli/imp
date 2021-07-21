#pragma once

#include "../system/GpuBuffer.h"

namespace imp {
  class Atmosphere;

  class AtmosphereBuffer {
  public:
    static constexpr auto STRIDE = vk::DeviceSize{256};
    static constexpr auto SIZE = vk::DeviceSize{60};

    explicit AtmosphereBuffer(
        gsl::not_null<VmaAllocator> allocator, std::size_t fenceCount);

    void update(Atmosphere const &atmosphere, std::size_t fenceIndex) noexcept;

    GpuBuffer const &getBuffer() const noexcept;

  private:
    GpuBuffer buffer_;
  };
} // namespace imp