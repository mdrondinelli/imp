#pragma once

#include <vulkan/vulkan.hpp>

namespace imp {
  class gpu_frame {
  public:
    gpu_frame(vk::Device device, uint32_t graphics_family);
    vk::Semaphore image_acquisition_semaphore() const noexcept;
    vk::Semaphore queue_submission_semaphore() const noexcept;
    vk::Fence queue_submission_fence() const noexcept;
    vk::CommandPool command_pool() const noexcept;
    vk::CommandBuffer command_buffer() const noexcept;

  private:
    vk::UniqueSemaphore image_acquisition_semaphore_;
    vk::UniqueSemaphore queue_submission_semaphore_;
    vk::UniqueFence queue_submission_fence_;
    vk::UniqueCommandPool command_pool_;
    vk::UniqueCommandBuffer command_buffer_;
  };
} // namespace imp