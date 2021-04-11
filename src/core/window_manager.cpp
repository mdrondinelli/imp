#include "window.h"

#include <stdexcept>

namespace imp {
  void init_windows() {
    if (!glfwInit())
      throw std::runtime_error{"Failed to initialize glfw."};
    std::atexit(glfwTerminate);
  }

  void poll_windows() {
    glfwPollEvents();
  }

  window_manager::window_manager(window_manager_create_info const &create_info):
      window_{create_window(create_info)},
      surface_{create_surface(create_info)} {}

  window_manager::~window_manager() {
    glfwDestroyWindow(window_);
  }

  GLFWwindow const *window_manager::get() const noexcept {
    return window_;
  }

  GLFWwindow *window_manager::get() noexcept {
    return window_;
  }

  vk::SurfaceKHR window_manager::surface() const noexcept {
    return *surface_;
  }

  int window_manager::width() const noexcept {
    int width;
    glfwGetWindowSize(window_, &width, nullptr);
    return width;
  }

  int window_manager::height() const noexcept {
    int height;
    glfwGetWindowSize(window_, nullptr, &height);
    return height;
  }

  int window_manager::framebuffer_width() const noexcept {
    int width;
    glfwGetFramebufferSize(window_, &width, nullptr);
    return width;
  }

  int window_manager::framebuffer_height() const noexcept {
    int height;
    glfwGetFramebufferSize(window_, nullptr, &height);
    return height;
  }

  bool window_manager::should_close() const noexcept {
    return glfwWindowShouldClose(window_);
  }

  GLFWwindow *
  window_manager::create_window(window_manager_create_info const &create_info) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(
        create_info.size[0],
        create_info.size[1],
        create_info.title,
        nullptr,
        nullptr);
    if (!window)
      throw std::runtime_error{"Failed to create window."};
    return window;
  }

  vk::UniqueSurfaceKHR window_manager::create_surface(
      window_manager_create_info const &create_info) {
    auto surface = VkSurfaceKHR{};
    if (glfwCreateWindowSurface(
            create_info.instance, window_, nullptr, &surface)) {
      throw std::runtime_error{"Failed to create vulkan surface."};
    }
    return vk::UniqueSurfaceKHR{surface, create_info.instance};
  }

  vk::Format select_format(window_manager_create_info const &create_info) {}

  vk::ColorSpaceKHR
  select_color_space(window_manager_create_info const &create_info) {}

  vk::PresentModeKHR
  select_present_mode(window_manager_create_info const &create_info) {}
} // namespace imp