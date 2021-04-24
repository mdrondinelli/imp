#include "window.h"

#include <iostream>
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

  window::window(gpu_context &context, vector2u const &size, char const *title):
      context_{&context},
      window_{create_window(size, title)},
      surface_{create_surface()},
      surface_format_{select_surface_format()},
      present_mode_{select_present_mode()},
      render_pass_{create_render_pass()} {
    if (context_->validation_enabled()) {
      context_->physical_device().getSurfaceSupportKHR(
          context_->present_family(), *surface_);
    }
    if (!context_->presentation_enabled()) {
      throw std::runtime_error{"failed to create window."};
    }
    create_swapchain();
  }

  window::~window() {
    destroy_swapchain();
    glfwDestroyWindow(window_);
  }

  GLFWwindow *window::create_window(vector2u const &size, char const *title) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(size[0], size[1], title, nullptr, nullptr);
    if (!window)
      throw std::runtime_error{"failed to create window."};
    return window;
  }

  vk::UniqueSurfaceKHR window::create_surface() {
    auto instance = context_->instance();
    auto surface = VkSurfaceKHR{};
    if (glfwCreateWindowSurface(instance, window_, nullptr, &surface)) {
      throw std::runtime_error{"failed to create vulkan surface."};
    }
    return vk::UniqueSurfaceKHR{surface, instance};
  }

  vk::SurfaceFormatKHR window::select_surface_format() {
    auto physical_device = context_->physical_device();
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

  vk::PresentModeKHR window::select_present_mode() {
    auto physical_device = context_->physical_device();
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

  vk::UniqueRenderPass window::create_render_pass() {
    auto color_attachment = vk::AttachmentDescription{};
    color_attachment.format = surface_format_.format;
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
    return context_->device().createRenderPassUnique(create_info);
  }

  void window::create_swapchain() {
    auto physical_device = context_->physical_device();
    auto device = context_->device();
    auto capabilities = physical_device.getSurfaceCapabilitiesKHR(*surface_);
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      swapchain_size_[0] = capabilities.currentExtent.width;
      swapchain_size_[1] = capabilities.currentExtent.height;
    } else {
      auto lo = make_vector(
          capabilities.minImageExtent.width,
          capabilities.minImageExtent.height);
      auto hi = make_vector(
          capabilities.maxImageExtent.width,
          capabilities.maxImageExtent.height);
      swapchain_size_ = clamp(framebuffer_size(), lo, hi);
    }
    auto queue_family_indices =
        std::array{context_->graphics_family(), context_->present_family()};
    auto create_info = vk::SwapchainCreateInfoKHR{};
    create_info.surface = *surface_;
    create_info.minImageCount = capabilities.minImageCount + 1;
    if (create_info.minImageCount > capabilities.maxImageCount &&
        capabilities.maxImageCount != 0) {
      create_info.minImageCount = capabilities.maxImageCount;
    }
    create_info.imageFormat = surface_format_.format;
    create_info.imageColorSpace = surface_format_.colorSpace;
    create_info.imageExtent.width = swapchain_size_[0];
    create_info.imageExtent.height = swapchain_size_[1];
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
    create_info.presentMode = present_mode_;
    create_info.clipped = true;
    swapchain_ = device.createSwapchainKHRUnique(create_info);
    swapchain_images_ = device.getSwapchainImagesKHR(*swapchain_);
    for (auto image : swapchain_images_) {
      auto view_info = vk::ImageViewCreateInfo{};
      view_info.image = image;
      view_info.viewType = vk::ImageViewType::e2D;
      view_info.format = surface_format_.format;
      view_info.components = vk::ComponentMapping{};
      view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      view_info.subresourceRange.baseMipLevel = 0;
      view_info.subresourceRange.levelCount = 1;
      view_info.subresourceRange.baseArrayLayer = 0;
      view_info.subresourceRange.layerCount = 1;
      swapchain_image_views_.emplace_back(
          device.createImageViewUnique(view_info));
    }
    for (auto &image_view : swapchain_image_views_) {
      auto framebuffer_info = vk::FramebufferCreateInfo{};
      framebuffer_info.renderPass = *render_pass_;
      auto attachments = std::array{*image_view};
      framebuffer_info.attachmentCount =
          static_cast<uint32_t>(attachments.size());
      framebuffer_info.pAttachments = attachments.data();
      framebuffer_info.width = swapchain_size_[0];
      framebuffer_info.height = swapchain_size_[1];
      framebuffer_info.layers = 1;
      swapchain_framebuffers_.emplace_back(
          device.createFramebufferUnique(framebuffer_info));
    }
  }

  void window::destroy_swapchain() {
    context_->device().waitIdle();
    swapchain_framebuffers_.clear();
    swapchain_image_views_.clear();
    swapchain_images_.clear();
    swapchain_.reset();
  }

  vk::Format window::format() const noexcept {
    return surface_format_.format;
  }

  vector2u window::window_size() const noexcept {
    int width;
    int height;
    glfwGetWindowSize(window_, &width, &height);
    return make_vector(width, height);
  }

  vector2u window::framebuffer_size() const noexcept {
    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);
    return make_vector(width, height);
  }

  vector2u const &window::swapchain_size() const noexcept {
    return swapchain_size_;
  }

  bool window::should_close() const noexcept {
    return glfwWindowShouldClose(window_);
  }

  vk::Framebuffer
  window::acquire_framebuffer(vk::Semaphore semaphore, vk::Fence fence) {
    try {
      auto index = context_->device()
                       .acquireNextImageKHR(
                           *swapchain_,
                           std::numeric_limits<uint64_t>::max(),
                           semaphore,
                           fence)
                       .value;
      return *swapchain_framebuffers_[index];
    } catch (vk::OutOfDateKHRError) {
      destroy_swapchain();
      create_swapchain();
      return acquire_framebuffer(semaphore, fence);
    }
  }

  void window::present_framebuffer(
      uint32_t wait_semaphore_count,
      vk::Semaphore const *wait_semaphores,
      vk::Framebuffer framebuffer) {
    auto info = vk::PresentInfoKHR{};
    info.waitSemaphoreCount = wait_semaphore_count;
    info.pWaitSemaphores = wait_semaphores;
    info.swapchainCount = 1;
    info.pSwapchains = &*swapchain_;
    auto it = std::find_if(
        swapchain_framebuffers_.begin(),
        swapchain_framebuffers_.end(),
        [=](auto &swapchain_framebuffer) {
          return *swapchain_framebuffer == framebuffer;
        });
    if (it == swapchain_framebuffers_.end()) {
      throw std::runtime_error{"failed to present framebuffer"};
    }
    auto idx = static_cast<uint32_t>(it - swapchain_framebuffers_.begin());
    info.pImageIndices = &idx;
    try {
      context_->present_queue().presentKHR(info);
    } catch (vk::OutOfDateKHRError) {
      destroy_swapchain();
      create_swapchain();
    }
  }
} // namespace imp