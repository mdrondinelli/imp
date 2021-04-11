#pragma once

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "window_manager_create_info.h"

namespace imp {
  void init_windows();
  void poll_windows();

  class window_manager {
  public:
    window_manager(window_manager_create_info const &create_info);
    ~window_manager();

    GLFWwindow const *get() const noexcept;
    GLFWwindow *get() noexcept;
    vk::SurfaceKHR surface() const noexcept;
    int width() const noexcept;
    int height() const noexcept;
    int framebuffer_width() const noexcept;
    int framebuffer_height() const noexcept;
    bool should_close() const noexcept;

  private:
    GLFWwindow *window_;
    vk::UniqueSurfaceKHR surface_;
    vk::Format format_;
    vk::ColorSpaceKHR color_space_;
    vk::PresentModeKHR present_mode_;

    GLFWwindow *create_window(window_manager_create_info const &create_info);
    vk::UniqueSurfaceKHR
    create_surface(window_manager_create_info const &create_info);
    vk::Format select_format(window_manager_create_info const &create_info);
    vk::ColorSpaceKHR
    select_color_space(window_manager_create_info const &create_info);
    vk::PresentModeKHR
    select_present_mode(window_manager_create_info const &create_info);
  };
} // namespace imp