#pragma once

#include "gpu_buffer.h"
#include "gpu_image.h"

namespace imp {
  class gpu_context {
  public:
    explicit gpu_context(bool validation_enabled, bool presentation_enabled);
    ~gpu_context();

    bool validation_enabled() const noexcept;
    bool presentation_enabled() const noexcept;
    vk::Instance instance() const noexcept;
    vk::PhysicalDevice physical_device() const noexcept;
    std::uint32_t graphics_family() const noexcept;
    std::uint32_t transfer_family() const noexcept;
    std::uint32_t present_family() const noexcept;
    vk::Device device() const noexcept;
    vk::Queue graphics_queue() const noexcept;
    vk::Queue transfer_queue() const noexcept;
    vk::Queue present_queue() const noexcept;

    gpu_buffer create_buffer(
        vk::BufferCreateInfo const &create_info,
        VmaAllocationCreateInfo const &alloc_info);
    gpu_image create_image(
        vk::ImageCreateInfo const &create_info,
        VmaAllocationCreateInfo const &alloc_info);

  private:
    bool validation_enabled_;
    bool presentation_enabled_;
    vk::UniqueInstance instance_;
    vk::PhysicalDevice physical_device_;
    std::uint32_t graphics_family_;
    std::uint32_t transfer_family_;
    std::uint32_t present_family_;
    vk::UniqueDevice device_;
    vk::Queue graphics_queue_;
    vk::Queue transfer_queue_;
    vk::Queue present_queue_;
    VmaAllocator allocator_;

    vk::UniqueInstance create_instance();
    vk::PhysicalDevice select_physical_device();
    std::uint32_t select_graphics_family();
    std::uint32_t select_transfer_family();
    std::uint32_t select_present_family();
    vk::UniqueDevice create_device();
    vk::Queue select_graphics_queue();
    vk::Queue select_transfer_queue();
    vk::Queue select_present_queue();
    VmaAllocator create_allocator();
  };
} // namespace imp