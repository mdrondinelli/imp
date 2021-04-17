#pragma once

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "../math/vector.h"
#include "gpu_context.h"

namespace imp {
  void init_windows();
  void poll_windows();

  class window {
  public:
    window(gpu_context &context, vector2u const &size, char const *title);
    ~window();

    vk::Format format() const noexcept;
    vector2u window_size() const noexcept;
    vector2u framebuffer_size() const noexcept;
    vector2u const &swapchain_size() const noexcept;
    bool should_close() const noexcept;

    vk::Framebuffer
    acquire_framebuffer(vk::Semaphore semaphore, vk::Fence fence);
    void present_framebuffer(
        uint32_t wait_semaphore_count,
        vk::Semaphore const *wait_semaphores,
        vk::Framebuffer framebuffer);

  private:
    gpu_context *context_;
    GLFWwindow *window_;
    vk::UniqueSurfaceKHR surface_;
    vk::SurfaceFormatKHR surface_format_;
    vk::PresentModeKHR present_mode_;
    vk::UniqueRenderPass render_pass_;
    vector2u swapchain_size_;
    vk::UniqueSwapchainKHR swapchain_;
    std::vector<vk::Image> swapchain_images_;
    std::vector<vk::UniqueImageView> swapchain_image_views_;
    std::vector<vk::UniqueFramebuffer> swapchain_framebuffers_;

    GLFWwindow *create_window(vector2u const &size, char const *title);
    vk::UniqueSurfaceKHR create_surface();
    vk::SurfaceFormatKHR select_surface_format();
    vk::UniqueRenderPass create_render_pass();
    vk::PresentModeKHR select_present_mode();

    void create_swapchain();
    void destroy_swapchain();
  };
} // namespace imp