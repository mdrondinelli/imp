#include "Display.h"

#include <iostream>
#include <stdexcept>

namespace imp {
  void Display::init() {
    if (!glfwInit())
      throw std::runtime_error{"Failed to initialize glfw."};
    std::atexit(glfwTerminate);
  }

  void Display::poll() {
    glfwPollEvents();
  }

  Display::Display(
      GpuContext *context,
      unsigned width,
      unsigned height,
      char const *title,
      bool fullscreen):
      context_{context},
      window_{createWindow(width, height, title, fullscreen)},
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

  Display::~Display() {
    destroySwapchain();
    glfwDestroyWindow(window_);
  }

  GLFWwindow *Display::createWindow(
      unsigned width, unsigned height, char const *title, bool fullscreen) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(
        width,
        height,
        title,
        fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr);
    if (!window)
      throw std::runtime_error{"failed to create window."};
    return window;
  }

  vk::UniqueSurfaceKHR Display::createSurface() {
    auto instance = context_->getInstance();
    auto surface = VkSurfaceKHR{};
    if (glfwCreateWindowSurface(instance, window_, nullptr, &surface)) {
      throw std::runtime_error{"failed to create vulkan surface."};
    }
    return vk::UniqueSurfaceKHR{surface, instance};
  }

  vk::SurfaceFormatKHR Display::selectSurfaceFormat() {
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

  vk::PresentModeKHR Display::selectPresentMode() {
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

  vk::UniqueRenderPass Display::createRenderPass() {
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

  void Display::createSwapchain() {
    auto physical_device = context_->getPhysicalDevice();
    auto device = context_->getDevice();
    auto capabilities = physical_device.getSurfaceCapabilitiesKHR(*surface_);
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      swapchainWidth_ = capabilities.currentExtent.width;
      swapchainHeight_ = capabilities.currentExtent.height;
    } else {
      swapchainWidth_ = std::clamp(
          getFramebufferWidth(),
          capabilities.minImageExtent.width,
          capabilities.maxImageExtent.width);
      swapchainHeight_ = std::clamp(
          getFramebufferWidth(),
          capabilities.minImageExtent.height,
          capabilities.maxImageExtent.height);
    }
    auto queue_family_indices =
        std::array{context_->getGraphicsFamily(), context_->getPresentFamily()};
    auto createInfo = vk::SwapchainCreateInfoKHR{};
    createInfo.surface = *surface_;
    createInfo.minImageCount = capabilities.minImageCount + 1;
    if (createInfo.minImageCount > capabilities.maxImageCount &&
        capabilities.maxImageCount != 0) {
      createInfo.minImageCount = capabilities.maxImageCount;
    }
    createInfo.imageFormat = surfaceFormat_.format;
    createInfo.imageColorSpace = surfaceFormat_.colorSpace;
    createInfo.imageExtent.width = swapchainWidth_;
    createInfo.imageExtent.height = swapchainHeight_;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    if (queue_family_indices[0] != queue_family_indices[1]) {
      createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      createInfo.queueFamilyIndexCount =
          static_cast<uint32_t>(queue_family_indices.size());
      createInfo.pQueueFamilyIndices = queue_family_indices.data();
    } else {
      createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode_;
    createInfo.clipped = true;
    swapchain_ = device.createSwapchainKHRUnique(createInfo);
    swapchainImages_ = device.getSwapchainImagesKHR(*swapchain_);
    for (auto image : swapchainImages_) {
      auto createInfo = vk::ImageViewCreateInfo{};
      createInfo.image = image;
      createInfo.viewType = vk::ImageViewType::e2D;
      createInfo.format = surfaceFormat_.format;
      createInfo.components = vk::ComponentMapping{};
      createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;
      swapchainImageViews_.emplace_back(
          device.createImageViewUnique(createInfo));
    }
    for (auto &imageView : swapchainImageViews_) {
      auto createInfo = vk::FramebufferCreateInfo{};
      createInfo.renderPass = *renderPass_;
      auto attachment = *imageView;
      createInfo.attachmentCount = 1;
      createInfo.pAttachments = &attachment;
      createInfo.width = swapchainWidth_;
      createInfo.height = swapchainHeight_;
      createInfo.layers = 1;
      swapchainFramebuffers_.emplace_back(
          device.createFramebufferUnique(createInfo));
    }
  }

  void Display::destroySwapchain() {
    context_->getDevice().waitIdle();
    swapchainFramebuffers_.clear();
    swapchainImageViews_.clear();
    swapchainImages_.clear();
    swapchain_.reset();
  }

  GpuContext *Display::getContext() const noexcept {
    return context_;
  }

  vk::SurfaceFormatKHR const &Display::getSurfaceFormat() const noexcept {
    return surfaceFormat_;
  }

  unsigned Display::getWindowWidth() const noexcept {
    int width;
    glfwGetWindowSize(window_, &width, nullptr);
    return static_cast<unsigned>(width);
  }

  unsigned Display::getWindowHeight() const noexcept {
    int height;
    glfwGetWindowSize(window_, nullptr, &height);
    return static_cast<unsigned>(height);
  }

  unsigned Display::getFramebufferWidth() const noexcept {
    int width;
    glfwGetFramebufferSize(window_, &width, nullptr);
    return static_cast<unsigned>(width);
  }

  unsigned Display::getFramebufferHeight() const noexcept {
    int height;
    glfwGetFramebufferSize(window_, nullptr, &height);
    return static_cast<unsigned>(height);
  }

  unsigned Display::getSwapchainWidth() const noexcept {
    return swapchainWidth_;
  }

  unsigned Display::getSwapchainHeight() const noexcept {
    return swapchainHeight_;
  }

  bool Display::shouldClose() const noexcept {
    return glfwWindowShouldClose(window_);
  }

  vk::Framebuffer
  Display::acquireFramebuffer(vk::Semaphore semaphore, vk::Fence fence) {
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

  void Display::presentFramebuffer(
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