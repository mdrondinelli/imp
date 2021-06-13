#pragma once

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "GpuContext.h"

namespace imp {
  void initWindows();
  void pollWindows();

  class Window {
  public:
    Window(
        GpuContext *context,
        unsigned width,
        unsigned height,
        char const *title,
        bool fullscreen);
    ~Window();

    GpuContext *getContext() const noexcept;
    vk::SurfaceFormatKHR const &getSurfaceFormat() const noexcept;
    unsigned getWindowWidth() const noexcept;
    unsigned getWindowHeight() const noexcept;
    unsigned getFramebufferWidth() const noexcept;
    unsigned getFramebufferHeight() const noexcept;
    unsigned getSwapchainWidth() const noexcept;
    unsigned getSwapchainHeight() const noexcept;
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
    unsigned swapchainWidth_;
    unsigned swapchainHeight_;
    vk::UniqueSwapchainKHR swapchain_;
    std::vector<vk::Image> swapchainImages_;
    std::vector<vk::UniqueImageView> swapchainImageViews_;
    std::vector<vk::UniqueFramebuffer> swapchainFramebuffers_;

    GLFWwindow *createWindow(
        unsigned width, unsigned height, char const *title, bool fullscreen);
    vk::UniqueSurfaceKHR createSurface();
    vk::SurfaceFormatKHR selectSurfaceFormat();
    vk::PresentModeKHR selectPresentMode();
    vk::UniqueRenderPass createRenderPass();

    void createSwapchain();
    void destroySwapchain();
  };
} // namespace imp