#include "gpu_frame.h"

namespace imp {
  gpu_frame::gpu_frame(vk::Device device, uint32_t graphics_family):
      image_acquisition_semaphore_{device.createSemaphoreUnique({})},
      queue_submission_semaphore_{device.createSemaphoreUnique({})},
      queue_submission_fence_{
          device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled})} {
    auto pool_info = vk::CommandPoolCreateInfo{};
    pool_info.flags = vk::CommandPoolCreateFlagBits::eTransient;
    pool_info.queueFamilyIndex = graphics_family;
    command_pool_ = device.createCommandPoolUnique(pool_info);
    auto buffer_info = vk::CommandBufferAllocateInfo{};
    buffer_info.commandPool = *command_pool_;
    buffer_info.level = vk::CommandBufferLevel::ePrimary;
    buffer_info.commandBufferCount = 1;
    auto command_buffers = device.allocateCommandBuffersUnique(buffer_info);
    command_buffer_ = std::move(command_buffers[0]);
  }

  vk::Semaphore gpu_frame::image_acquisition_semaphore() const noexcept {
    return *image_acquisition_semaphore_;
  }

  vk::Semaphore gpu_frame::queue_submission_semaphore() const noexcept {
    return *queue_submission_semaphore_;
  }

  vk::Fence gpu_frame::queue_submission_fence() const noexcept {
    return *queue_submission_fence_;
  }

  vk::CommandPool gpu_frame::command_pool() const noexcept {
    return *command_pool_;
  }

  vk::CommandBuffer gpu_frame::command_buffer() const noexcept {
    return *command_buffer_;
  }
} // namespace imp