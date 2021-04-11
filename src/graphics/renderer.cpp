#include "renderer.h"

namespace imp {
  renderer::renderer(renderer_create_info const &create_info):
      gpu_manager_{create_info.gpu_manager},
      window_manager_{create_info.window_manager},
      compute_command_pool_{create_compute_command_pool()} {}

  vk::UniqueCommandPool renderer::create_compute_command_pool() {
    auto info = vk::CommandPoolCreateInfo{};
    info.flags = vk::CommandPoolCreateFlagBits::eTransient;
    info.queueFamilyIndex = gpu_manager_->compute_family();
    return gpu_manager_->logical_device().createCommandPoolUnique(info);
  }

  vk::UniqueRenderPass renderer::create_atmosphere_pass() {
    auto attachments = std::array<vk::AttachmentDescription, 1>{};
    auto &color_attachment = attachments[0];
    color_attachment.format = gpu_manager_->physical_device().swapchain_image_format;
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
    return logical_device_.handle->createRenderPassUnique(info);
  }

  vk::UniqueDescriptorSetLayout create_compute_lut_descriptor_set_layout();

  vk::UniqueDescriptorSetLayout create_atmosphere_descriptor_set_layout();

  vk::UniqueShaderModule
  create_shader_module(std::filesystem::path const &path);

  vk::UniquePipelineLayout create_scattering_pipeline_layout();

  vk::UniquePipelineLayout create_optical_depth_pipeline_layout();

  vk::UniquePipelineLayout create_atmosphere_pipeline_layout();

  vk::UniquePipeline create_scattering_pipeline();

  vk::UniquePipeline create_optical_depth_pipeline();

  vk::UniquePipeline create_atmosphere_pipeline();

  vk::UniqueDescriptorPool create_descriptor_pool();

  vk::UniqueDescriptorSet allocate_compute_lut_descriptor_set();

  vk::UniqueDescriptorSet allocate_atmosphere_descriptor_set();

  vk::Sampler create_lut_sampler();

  scattering_lut create_scattering_lut();

  optical_depth_lut create_optical_depth_lut();

  std::vector<gpu_frame> create_frames();
} // namespace imp