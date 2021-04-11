#pragma once

#include <vulkan/vulkan.hpp>

#include "gpu_allocator.h"
#include "gpu_manager_create_info.h"

namespace imp {
  class gpu_manager {
  public:
    gpu_manager(gpu_manager_create_info const &create_info);

    bool validation_enabled() const noexcept;
    bool presentation_enabled() const noexcept;
    vk::Instance instance() const noexcept;
    vk::PhysicalDevice physical_device() const noexcept;
    uint32_t graphics_family() const noexcept;
    uint32_t compute_family() const noexcept;
    uint32_t present_family() const noexcept;
    vk::Device logical_device() const noexcept;
    vk::Queue graphics_queue() const noexcept;
    vk::Queue compute_queue() const noexcept;
    vk::Queue present_queue() const noexcept;
    gpu_allocator const &allocator() const noexcept;
    gpu_allocator &allocator() noexcept;

  private:
    struct physical_device_info {
      vk::PhysicalDevice handle;
      uint32_t graphics_family;
      uint32_t compute_family;
      uint32_t present_family;
    };

    struct logical_device_info {
      vk::UniqueDevice handle;
      vk::Queue graphics_queue;
      vk::Queue compute_queue;
      vk::Queue present_queue;
    };

    bool validation_enabled_;
    bool presentation_enabled_;
    vk::UniqueInstance instance_;
    physical_device_info physical_device_;
    logical_device_info logical_device_;
    gpu_allocator allocator_;

    vk::UniqueInstance create_instance();
    physical_device_info select_physical_device();
    logical_device_info create_logical_device();
    gpu_allocator create_allocator();
  };
} // namespace imp