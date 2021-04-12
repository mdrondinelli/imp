#pragma once

#include "gpu_buffer.h"
#include "gpu_image.h"

namespace imp {
  class gpu_context;
  class gpu_allocator {
  public:
    gpu_allocator(gpu_context &context);
    ~gpu_allocator();

    gpu_buffer create_buffer(
        vk::BufferCreateInfo const &buffer_info,
        VmaAllocationCreateInfo const &allocation_info);
    gpu_image create_image(
        vk::ImageCreateInfo const &image_info,
        VmaAllocationCreateInfo const &allocation_info);

  private:
    vk::Instance instance_;
    vk::PhysicalDevice physical_device_;
    vk::Device device_;
    VmaAllocator allocator_;
  };
} // namespace imp