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
    create_swapchain();
  }

  window::~window() {
    glfwDestroyWindow(window_);
  }

  GLFWwindow *window::create_window(vector2u const &size, char const *title) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(size[0], size[1], title, nullptr, nullptr);
    if (!window)
      throw std::runtime_error{"Failed to create window."};
    return window;
  }

  vk::UniqueSurfaceKHR window::create_surface() {
    auto instance = context_->instance();
    auto surface = VkSurfaceKHR{};
    if (glfwCreateWindowSurface(instance, window_, nullptr, &surface)) {
      throw std::runtime_error{"Failed to create vulkan surface."};
    }
    return vk::UniqueSurfaceKHR{surface, instance};
  }

  vk::SurfaceFormatKHR window::select_surface_format() {
    auto physical_device = context_->physical_device();
    auto formats = physical_device.getSurfaceFormatsKHR(*surface_);
    auto format = formats[0];
    for (auto i = size_t{1}; i < formats.size(); ++i) {
      if (formats[i].format == vk::Format::eB8G8R8A8Srgb &&
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
      if (priority(present_mode) < priority(present_modes[i]))
        present_mode = present_modes[i];
    }
    return present_mode;
  }

  vk::UniqueRenderPass window::create_render_pass() {
    auto attachments = std::array<vk::AttachmentDescription, 1>{};
    auto &color_attachment = attachments[0];
    color_attachment.format = surface_format_.format;
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    auto subpass_color_attachments = std::array<vk::AttachmentReference, 1>{};
    auto &subpass_color_attachment = subpass_color_attachments[0];
    subpass_color_attachment.attachment = 0;
    subpass_color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpasses = std::array<vk::SubpassDescription, 1>{};
    auto &subpass = subpasses[0];
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount =
        static_cast<uint32_t>(subpass_color_attachments.size());
    subpass.pColorAttachments = subpass_color_attachments.data();
    auto info = vk::RenderPassCreateInfo{};
    info.attachmentCount = static_cast<uint32_t>(attachments.size());
    info.pAttachments = attachments.data();
    info.subpassCount = static_cast<uint32_t>(subpasses.size());
    info.pSubpasses = subpasses.data();
    return context_->device().createRenderPassUnique(info);
  }

  void window::create_swapchain() {
    auto physical_device = context_->physical_device();
    auto logical_device = context_->device();
    logical_device.waitIdle();
    swapchain_framebuffers_.clear();
    swapchain_image_views_.clear();
    swapchain_images_.clear();
    swapchain_.reset();
    auto info = vk::SwapchainCreateInfoKHR{};
    info.surface = *surface_;
    auto capabilities = physical_device.getSurfaceCapabilitiesKHR(*surface_);
    info.minImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount != 0 &&
        info.minImageCount > capabilities.maxImageCount) {
      info.minImageCount = capabilities.maxImageCount;
    }
    info.imageFormat = surface_format_.format;
    info.imageColorSpace = surface_format_.colorSpace;
    if (capabilities.currentExtent.width ==
        std::numeric_limits<uint32_t>::max()) {
      int width;
      int height;
      glfwGetFramebufferSize(window_, &width, &height);
      swapchain_size_[0] = width;
      swapchain_size_[1] = height;
    } else {
      swapchain_size_[0] = capabilities.currentExtent.width;
      swapchain_size_[1] = capabilities.currentExtent.height;
    }
    info.imageExtent.width = swapchain_size_[0];
    info.imageExtent.height = swapchain_size_[1];
    info.imageArrayLayers = 1;
    info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    auto queue_family_indices =
        std::array{context_->graphics_family(), context_->present_family()};
    if (queue_family_indices[0] == queue_family_indices[1]) {
      info.imageSharingMode = vk::SharingMode::eExclusive;
    } else {
      info.imageSharingMode = vk::SharingMode::eConcurrent;
      info.queueFamilyIndexCount =
          static_cast<uint32_t>(queue_family_indices.size());
      info.pQueueFamilyIndices = queue_family_indices.data();
    }
    info.preTransform = capabilities.currentTransform;
    info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    info.presentMode = present_mode_;
    info.clipped = true;
    swapchain_ = logical_device.createSwapchainKHRUnique(info);
    swapchain_images_ = logical_device.getSwapchainImagesKHR(*swapchain_);
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
          logical_device.createImageViewUnique(view_info));
    }
    for (auto &image_view : swapchain_image_views_) {
      auto framebuffer_info = vk::FramebufferCreateInfo{};
      framebuffer_info.renderPass = *render_pass_;
      auto attachments = std::array{*image_view};
      framebuffer_info.attachmentCount =
          static_cast<uint32_t>(attachments.size());
      framebuffer_info.pAttachments = attachments.data();
      framebuffer_info.width = info.imageExtent.width;
      framebuffer_info.height = info.imageExtent.height;
      framebuffer_info.layers = 1;
      swapchain_framebuffers_.emplace_back(
          logical_device.createFramebufferUnique(framebuffer_info));
    }
  }

  vk::SurfaceKHR window::surface() const noexcept {
    return *surface_;
  }

  vk::SurfaceFormatKHR window::surface_format() const noexcept {
    return surface_format_;
  }

  vk::PresentModeKHR window::present_mode() const noexcept {
    return present_mode_;
  }

  vk::RenderPass window::render_pass() const noexcept {
    return *render_pass_;
  }

  vector2u const &window::swapchain_size() const noexcept {
    return swapchain_size_;
  }

  int window::width() const noexcept {
    int width;
    glfwGetWindowSize(window_, &width, nullptr);
    return width;
  }

  int window::height() const noexcept {
    int height;
    glfwGetWindowSize(window_, nullptr, &height);
    return height;
  }

  int window::framebuffer_width() const noexcept {
    int width;
    glfwGetFramebufferSize(window_, &width, nullptr);
    return width;
  }

  int window::framebuffer_height() const noexcept {
    int height;
    glfwGetFramebufferSize(window_, nullptr, &height);
    return height;
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
      create_swapchain();
    }
  }
} // namespace imp