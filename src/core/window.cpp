#include "Window.h"

#include <iostream>
#include <stdexcept>

namespace imp {
  void initWindows() {
    if (!glfwInit())
      throw std::runtime_error{"Failed to initialize glfw."};
    std::atexit(glfwTerminate);
  }

  void pollWindows() {
    glfwPollEvents();
  }

  Window::Window(WindowCreateInfo const &createInfo):
      context_{createInfo.context},
      window_{createWindow(createInfo.size, createInfo.title)},
      surface_{createSurface()},
      surfaceFormat_{selectSurfaceFormat()},
      presentMode_{selectPresentMode()},
      renderPass_{createRenderPass()} {
    if (context_->isValidationEnabled()) {
      context_->getPhysicalDevice().getSurfaceSupportKHR(
          context_->getPresentFamily(), *surface_);
    }
    if (!context_->isPresentationEnabled()) {
      throw std::runtime_error{"failed to create window."};
    }
    createSwapchain();
  }

  Window::~Window() {
    destroySwapchain();
    glfwDestroyWindow(window_);
  }

  GLFWwindow *Window::createWindow(Vector2u const &size, char const *title) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(size[0], size[1], title, nullptr, nullptr);
    if (!window)
      throw std::runtime_error{"failed to create window."};
    return window;
  }

  vk::UniqueSurfaceKHR Window::createSurface() {
    auto instance = context_->getInstance();
    auto surface = VkSurfaceKHR{};
    if (glfwCreateWindowSurface(instance, window_, nullptr, &surface)) {
      throw std::runtime_error{"failed to create vulkan surface."};
    }
    return vk::UniqueSurfaceKHR{surface, instance};
  }

  vk::SurfaceFormatKHR Window::selectSurfaceFormat() {
    auto physical_device = context_->getPhysicalDevice();
    auto formats = physical_device.getSurfaceFormatsKHR(*surface_);
    auto format = formats[0];
    for (auto i = size_t{1}; i < formats.size(); ++i) {
      if (formats[i].format == vk::Format::eB8G8R8A8Unorm &&
          formats[i].colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        format = formats[i];
      }
    }
    return format;
  }

  vk::PresentModeKHR Window::selectPresentMode() {
    auto physical_device = context_->getPhysicalDevice();
    auto present_modes = physical_device.getSurfacePresentModesKHR(*surface_);
    auto present_mode = present_modes[0];
    for (auto i = size_t{1}; i < present_modes.size(); ++i) {
      auto priority = [](vk::PresentModeKHR mode) {
        switch (mode) {
        default:
        case vk::PresentModeKHR::eFifo:
          return 0;
        case vk::PresentModeKHR::eFifoRelaxed:
          return 1;
        case vk::PresentModeKHR::eImmediate:
          return 2;
        case vk::PresentModeKHR::eMailbox:
          return 3;
        }
      };
      if (priority(present_modes[i]) > priority(present_mode)) {
        present_mode = present_modes[i];
      }
    }
    return present_mode;
  }

  vk::UniqueRenderPass Window::createRenderPass() {
    auto color_attachment = vk::AttachmentDescription{};
    color_attachment.format = surfaceFormat_.format;
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    auto subpass_color_attachment = vk::AttachmentReference{};
    subpass_color_attachment.attachment = 0;
    subpass_color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = vk::SubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &subpass_color_attachment;
    auto create_info = vk::RenderPassCreateInfo{};
    create_info.attachmentCount = 1;
    create_info.pAttachments = &color_attachment;
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;
    return context_->getDevice().createRenderPassUnique(create_info);
  }

  void Window::createSwapchain() {
    auto physical_device = context_->getPhysicalDevice();
    auto device = context_->getDevice();
    auto capabilities = physical_device.getSurfaceCapabilitiesKHR(*surface_);
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      swapchainSize_[0] = capabilities.currentExtent.width;
      swapchainSize_[1] = capabilities.currentExtent.height;
    } else {
      auto lo = makeVector(
          capabilities.minImageExtent.width,
          capabilities.minImageExtent.height);
      auto hi = makeVector(
          capabilities.maxImageExtent.width,
          capabilities.maxImageExtent.height);
      swapchainSize_ = clamp(getFramebufferSize(), lo, hi);
    }
    auto queue_family_indices =
        std::array{context_->getGraphicsFamily(), context_->getPresentFamily()};
    auto create_info = vk::SwapchainCreateInfoKHR{};
    create_info.surface = *surface_;
    create_info.minImageCount = capabilities.minImageCount + 1;
    if (create_info.minImageCount > capabilities.maxImageCount &&
        capabilities.maxImageCount != 0) {
      create_info.minImageCount = capabilities.maxImageCount;
    }
    create_info.imageFormat = surfaceFormat_.format;
    create_info.imageColorSpace = surfaceFormat_.colorSpace;
    create_info.imageExtent.width = swapchainSize_[0];
    create_info.imageExtent.height = swapchainSize_[1];
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    if (queue_family_indices[0] != queue_family_indices[1]) {
      create_info.imageSharingMode = vk::SharingMode::eConcurrent;
      create_info.queueFamilyIndexCount =
          static_cast<uint32_t>(queue_family_indices.size());
      create_info.pQueueFamilyIndices = queue_family_indices.data();
    } else {
      create_info.imageSharingMode = vk::SharingMode::eExclusive;
    }
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    create_info.presentMode = presentMode_;
    create_info.clipped = true;
    swapchain_ = device.createSwapchainKHRUnique(create_info);
    swapchainImages_ = device.getSwapchainImagesKHR(*swapchain_);
    for (auto image : swapchainImages_) {
      auto view_info = vk::ImageViewCreateInfo{};
      view_info.image = image;
      view_info.viewType = vk::ImageViewType::e2D;
      view_info.format = surfaceFormat_.format;
      view_info.components = vk::ComponentMapping{};
      view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      view_info.subresourceRange.baseMipLevel = 0;
      view_info.subresourceRange.levelCount = 1;
      view_info.subresourceRange.baseArrayLayer = 0;
      view_info.subresourceRange.layerCount = 1;
      swapchainImageViews_.emplace_back(
          device.createImageViewUnique(view_info));
    }
    for (auto &image_view : swapchainImageViews_) {
      auto framebuffer_info = vk::FramebufferCreateInfo{};
      framebuffer_info.renderPass = *renderPass_;
      auto attachments = std::array{*image_view};
      framebuffer_info.attachmentCount =
          static_cast<uint32_t>(attachments.size());
      framebuffer_info.pAttachments = attachments.data();
      framebuffer_info.width = swapchainSize_[0];
      framebuffer_info.height = swapchainSize_[1];
      framebuffer_info.layers = 1;
      swapchainFramebuffers_.emplace_back(
          device.createFramebufferUnique(framebuffer_info));
    }
  }

  void Window::destroySwapchain() {
    context_->getDevice().waitIdle();
    swapchainFramebuffers_.clear();
    swapchainImageViews_.clear();
    swapchainImages_.clear();
    swapchain_.reset();
  }

  vk::Format Window::getFormat() const noexcept {
    return surfaceFormat_.format;
  }

  vk::ColorSpaceKHR Window::getColorSpace() const noexcept {
    return surfaceFormat_.colorSpace;
  }

  Vector2u Window::getWindowSize() const noexcept {
    int width;
    int height;
    glfwGetWindowSize(window_, &width, &height);
    return makeVector(width, height);
  }

  Vector2u Window::getFramebufferSize() const noexcept {
    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);
    return makeVector(width, height);
  }

  Vector2u const &Window::getSwapchainSize() const noexcept {
    return swapchainSize_;
  }

  bool Window::shouldClose() const noexcept {
    return glfwWindowShouldClose(window_);
  }

  vk::Framebuffer
  Window::acquireFramebuffer(vk::Semaphore semaphore, vk::Fence fence) {
    try {
      auto index = context_->getDevice()
                       .acquireNextImageKHR(
                           *swapchain_,
                           std::numeric_limits<uint64_t>::max(),
                           semaphore,
                           fence)
                       .value;
      return *swapchainFramebuffers_[index];
    } catch (vk::OutOfDateKHRError) {
      destroySwapchain();
      createSwapchain();
      return acquireFramebuffer(semaphore, fence);
    }
  }

  void Window::presentFramebuffer(
      uint32_t wait_semaphore_count,
      vk::Semaphore const *wait_semaphores,
      vk::Framebuffer framebuffer) {
    auto info = vk::PresentInfoKHR{};
    info.waitSemaphoreCount = wait_semaphore_count;
    info.pWaitSemaphores = wait_semaphores;
    info.swapchainCount = 1;
    info.pSwapchains = &*swapchain_;
    auto it = std::find_if(
        swapchainFramebuffers_.begin(),
        swapchainFramebuffers_.end(),
        [=](auto &swapchain_framebuffer) {
          return *swapchain_framebuffer == framebuffer;
        });
    if (it == swapchainFramebuffers_.end()) {
      throw std::runtime_error{"failed to present framebuffer"};
    }
    auto idx = static_cast<uint32_t>(it - swapchainFramebuffers_.begin());
    info.pImageIndices = &idx;
    try {
      context_->getPresentQueue().presentKHR(info);
    } catch (vk::OutOfDateKHRError) {
      destroySwapchain();
      createSwapchain();
    }
  }
} // namespace imp