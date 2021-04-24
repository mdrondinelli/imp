#pragma once

#include "GpuBuffer.h"
#include "GpuImage.h"

namespace imp {
  struct GpuContextCreateInfo {
    bool validation;
    bool presentation;
  };

  class GpuContext {
  public:
    explicit GpuContext(GpuContextCreateInfo const &createInfo);
    ~GpuContext();

    bool isValidationEnabled() const noexcept;
    bool isPresentationEnabled() const noexcept;
    vk::Instance getInstance() const noexcept;
    vk::PhysicalDevice getPhysicalDevice() const noexcept;
    std::uint32_t getGraphicsFamily() const noexcept;
    std::uint32_t getTransferFamily() const noexcept;
    std::uint32_t getPresentFamily() const noexcept;
    vk::Device getDevice() const noexcept;
    vk::Queue getGraphicsQueue() const noexcept;
    vk::Queue getTransferQueue() const noexcept;
    vk::Queue getPresentQueue() const noexcept;

    GpuBuffer createBuffer(
        vk::BufferCreateInfo const &bufferCreateInfo,
        VmaAllocationCreateInfo const &allocationCreateInfo);
    GpuImage createImage(
        vk::ImageCreateInfo const &imageCreateInfo,
        VmaAllocationCreateInfo const &allocationCreateInfo);

  private:
    bool validationEnabled_;
    bool presentationEnabled_;
    vk::UniqueInstance instance_;
    vk::PhysicalDevice physicalDevice_;
    std::uint32_t graphicsFamily_;
    std::uint32_t transferFamily_;
    std::uint32_t presentFamily_;
    vk::UniqueDevice device_;
    vk::Queue graphicsQueue_;
    vk::Queue transferQueue_;
    vk::Queue presentQueue_;
    VmaAllocator allocator_;

    vk::UniqueInstance createInstance();
    vk::PhysicalDevice selectPhysicalDevice();
    std::uint32_t selectGraphicsFamily();
    std::uint32_t selectTransferFamily();
    std::uint32_t selectPresentFamily();
    vk::UniqueDevice createDevice();
    vk::Queue selectGraphicsQueue();
    vk::Queue selectTransferQueue();
    vk::Queue selectPresentQueue();
    VmaAllocator createAllocator();
  };
} // namespace imp