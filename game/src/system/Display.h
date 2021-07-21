#pragma once

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "GpuContext.h"

namespace imp {
  class Display {
  public:
    static void init();
    static void poll();

    Display(
        gsl::not_null<GpuContext *> context,
        unsigned width,
        unsigned height,
        char const *title,
        bool fullscreen);
    ~Display();

    gsl::not_null<GpuContext *> getContext() const noexcept;
    vk::SurfaceFormatKHR const &getSurfaceFormat() const noexcept;
    unsigned getWindowWidth() const noexcept;
    unsigned getWindowHeight() const noexcept;
    unsigned getFramebufferWidth() const noexcept;
    unsigned getFramebufferHeight() const noexcept;
    unsigned getSwapchainWidth() const noexcept;
    unsigned getSwapchainHeight() const noexcept;
    bool shouldClose() const noexcept;

    std::uint32_t acquireImage(vk::Semaphore semaphore, vk::Fence fence);
    void present(
        gsl::span<vk::Semaphore const> waitSemaphores,
        std::uint32_t imageIndex);

    vk::RenderPass getRenderPass() const noexcept;
    vk::Framebuffer getFramebuffer(std::uint32_t index) const noexcept;

  private:
    gsl::not_null<GpuContext *> context_;
    GLFWwindow *window_;
    vk::SurfaceKHR surface_;
    vk::SurfaceFormatKHR surfaceFormat_;
    vk::PresentModeKHR presentMode_;
    vk::RenderPass renderPass_;
    unsigned swapchainWidth_;
    unsigned swapchainHeight_;
    vk::SwapchainKHR swapchain_;
    std::vector<vk::Image> swapchainImages_;
    std::vector<vk::ImageView> swapchainImageViews_;
    std::vector<vk::Framebuffer> swapchainFramebuffers_;

    GLFWwindow *createWindow(
        unsigned width, unsigned height, char const *title, bool fullscreen);
    vk::SurfaceKHR createSurface();
    vk::SurfaceFormatKHR selectSurfaceFormat();
    vk::PresentModeKHR selectPresentMode();
    vk::RenderPass createRenderPass();

    void createSwapchain();
    void destroySwapchain();
  };
} // namespace imp