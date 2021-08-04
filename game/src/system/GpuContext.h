#pragma once

#include <mutex>

#include <vulkan/vulkan.hpp>

#include "../util/Gsl.h"
#include "GpuDescriptorSetLayoutCache.h"
#include "GpuPipelineLayoutCache.h"
#include "GpuRenderPassCache.h"
#include "GpuSamplerCache.h"
#include "vk_mem_alloc.h"

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
    std::uint32_t getComputeFamily() const noexcept;
    std::optional<std::uint32_t> getTransferFamily() const noexcept;
    std::optional<std::uint32_t> getPresentFamily() const noexcept;
    vk::Device getDevice() const noexcept;
    vk::Queue getGraphicsQueue() const noexcept;
    vk::Queue getComputeQueue() const noexcept;
    vk::Queue getTransferQueue() const noexcept;
    vk::Queue getPresentQueue() const noexcept;
    gsl::not_null<VmaAllocator> getAllocator() const noexcept;

    vk::RenderPass createRenderPass(GpuRenderPassCreateInfo const &createInfo);

    vk::DescriptorSetLayout createDescriptorSetLayout(
        GpuDescriptorSetLayoutCreateInfo const &createInfo);

    vk::PipelineLayout
    createPipelineLayout(GpuPipelineLayoutCreateInfo const &createInfo);

    vk::Sampler createSampler(GpuSamplerCreateInfo const &createInfo);

  private:
    bool validationEnabled_;
    bool presentationEnabled_;
    vk::UniqueInstance instance_;
    vk::PhysicalDevice physicalDevice_;
    std::uint32_t graphicsFamily_;
    std::uint32_t computeFamily_;
    std::optional<std::uint32_t> transferFamily_;
    std::optional<std::uint32_t> presentFamily_;
    vk::UniqueDevice device_;
    vk::Queue graphicsQueue_;
    vk::Queue computeQueue_;
    vk::Queue transferQueue_;
    vk::Queue presentQueue_;
    gsl::not_null<VmaAllocator> allocator_;
    GpuRenderPassCache renderPasses_;
    GpuDescriptorSetLayoutCache descriptorSetLayouts_;
    GpuPipelineLayoutCache pipelineLayouts_;
    GpuSamplerCache samplers_;

    vk::UniqueInstance createInstance();
    vk::PhysicalDevice selectPhysicalDevice();
    std::uint32_t selectGraphicsFamily();
    std::uint32_t selectComputeFamily();
    std::optional<std::uint32_t> selectTransferFamily();
    std::optional<std::uint32_t> selectPresentFamily();
    vk::UniqueDevice createDevice();
    vk::Queue selectGraphicsQueue();
    vk::Queue selectComputeQueue();
    vk::Queue selectTransferQueue();
    vk::Queue selectPresentQueue();
    gsl::not_null<VmaAllocator> createAllocator();
  };
} // namespace imp