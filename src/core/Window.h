#pragma once

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "../math/Vector.h"
#include "GpuContext.h"

namespace imp {
  void initWindows();
  void pollWindows();

  struct WindowCreateInfo {
    GpuContext *context;
    Vector2u size;
    char const *title;
    bool fullscreen;
  };

  class Window {
  public:
    explicit Window(WindowCreateInfo const &createInfo);
    ~Window();

    GpuContext *getContext() const noexcept;
    vk::SurfaceFormatKHR const &getSurfaceFormat() const noexcept;
    Vector2u getWindowSize() const noexcept;
    Vector2u getFramebufferSize() const noexcept;
    Vector2u const &getSwapchainSize() const noexcept;
    bool shouldClose() const noexcept;

    vk::Framebuffer
    acquireFramebuffer(vk::Semaphore semaphore, vk::Fence fence);
    void presentFramebuffer(
        std::uint32_t waitSemaphoreCount,
        vk::Semaphore const *waitSemaphores,
        vk::Framebuffer framebuffer);

  private:
    GpuContext *context_;
    GLFWwindow *window_;
    vk::UniqueSurfaceKHR surface_;
    vk::SurfaceFormatKHR surfaceFormat_;
    vk::PresentModeKHR presentMode_;
    vk::UniqueRenderPass renderPass_;
    Vector2u swapchainSize_;
    vk::UniqueSwapchainKHR swapchain_;
    std::vector<vk::Image> swapchainImages_;
    std::vector<vk::UniqueImageView> swapchainImageViews_;
    std::vector<vk::UniqueFramebuffer> swapchainFramebuffers_;

    GLFWwindow *
    createWindow(Vector2u const &size, char const *title, bool fullscreen);
    vk::UniqueSurfaceKHR createSurface();
    vk::SurfaceFormatKHR selectSurfaceFormat();
    vk::PresentModeKHR selectPresentMode();
    vk::UniqueRenderPass createRenderPass();

    void createSwapchain();
    void destroySwapchain();
  };
} // namespace imp