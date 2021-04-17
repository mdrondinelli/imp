#include "renderer.h"

#include <fstream>

#include "../math/matrix.h"

namespace imp {
  renderer::renderer(gpu_context &context, window &window):
      context_{&context},
      window_{&window},
      atmosphere_{context},
      atmosphere_pass_{create_atmosphere_pass()},
      atmosphere_descriptor_set_layout_{
          create_atmosphere_descriptor_set_layout()},
      atmosphere_pipeline_layout_{create_atmosphere_pipeline_layout()},
      atmosphere_pipeline_{create_atmosphere_pipeline()},
      descriptor_pool_{create_descriptor_pool()},
      atmosphere_descriptor_set_{allocate_atmosphere_descriptor_set()},
      lut_sampler_{create_lut_sampler()},
      frames_{create_frames()} {
    update_atmosphere_descriptor_set();
  }

  renderer::~renderer() {
    context_->device().waitIdle();
  }

  vk::UniqueRenderPass renderer::create_atmosphere_pass() {
    auto attachments = std::array<vk::AttachmentDescription, 1>{};
    auto &color_attachment = attachments[0];
    color_attachment.format = window_->format();
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

  vk::UniqueDescriptorSetLayout
  renderer::create_atmosphere_descriptor_set_layout() {
    auto scattering_binding = vk::DescriptorSetLayoutBinding{};
    scattering_binding.binding = 0;
    scattering_binding.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    scattering_binding.descriptorCount = 1;
    scattering_binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto optical_depth_binding = vk::DescriptorSetLayoutBinding{};
    optical_depth_binding.binding = 1;
    optical_depth_binding.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    optical_depth_binding.descriptorCount = 1;
    optical_depth_binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto bindings = std::array{scattering_binding, optical_depth_binding};
    auto info = vk::DescriptorSetLayoutCreateInfo{};
    info.bindingCount = static_cast<uint32_t>(bindings.size());
    info.pBindings = bindings.data();
    return context_->device().createDescriptorSetLayoutUnique(info);
  }

  vk::UniqueShaderModule
  renderer::create_shader_module(std::filesystem::path const &path) {
    auto code = std::vector<char>{};
    auto in = std::ifstream{};
    in.exceptions(std::ios::badbit | std::ios::failbit);
    in.open(path, std::ios::binary);
    in.seekg(0, std::ios::end);
    auto size = static_cast<size_t>(in.tellg());
    if (size % sizeof(uint32_t) != 0) {
      throw std::runtime_error{"failed to create shader module"};
    }
    code.resize(size);
    in.seekg(0, std::ios::beg);
    in.read(code.data(), code.size());
    auto module_info = vk::ShaderModuleCreateInfo{};
    module_info.codeSize = static_cast<uint32_t>(code.size());
    module_info.pCode = reinterpret_cast<uint32_t *>(code.data());
    return context_->device().createShaderModuleUnique(module_info);
  }

  vk::UniquePipelineLayout renderer::create_atmosphere_pipeline_layout() {
    auto set_layouts = std::array{*atmosphere_descriptor_set_layout_};
    auto push_constant_range = vk::PushConstantRange{};
    push_constant_range.stageFlags =
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    push_constant_range.offset = 0;
    push_constant_range.size = 124;
    auto info = vk::PipelineLayoutCreateInfo{};
    info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &push_constant_range;
    return context_->device().createPipelineLayoutUnique(info);
  }

  vk::UniquePipeline renderer::create_atmosphere_pipeline() {
    auto vert_module = create_shader_module("./res/atmosphere-vert.spv");
    auto vert_stage = vk::PipelineShaderStageCreateInfo{};
    vert_stage.stage = vk::ShaderStageFlagBits::eVertex;
    vert_stage.module = *vert_module;
    vert_stage.pName = "main";
    auto frag_module = create_shader_module("./res/atmosphere-frag.spv");
    auto frag_stage = vk::PipelineShaderStageCreateInfo{};
    frag_stage.stage = vk::ShaderStageFlagBits::eFragment;
    frag_stage.module = *frag_module;
    frag_stage.pName = "main";
    auto stages = std::array{vert_stage, frag_stage};
    auto vertex_input_state = vk::PipelineVertexInputStateCreateInfo{};
    auto input_assembly_state = vk::PipelineInputAssemblyStateCreateInfo{};
    input_assembly_state.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewport_state = vk::PipelineViewportStateCreateInfo{};
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    auto rasterization_state = vk::PipelineRasterizationStateCreateInfo{};
    rasterization_state.polygonMode = vk::PolygonMode::eFill;
    rasterization_state.cullMode = vk::CullModeFlagBits::eNone;
    rasterization_state.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization_state.lineWidth = 1.0f;
    auto multisample_state = vk::PipelineMultisampleStateCreateInfo{};
    multisample_state.rasterizationSamples = vk::SampleCountFlagBits::e1;
    // auto depth_stencil_state = vk::PipelineDepthStencilStateCreateInfo{};
    // depth_stencil_state.depthTestEnable = false;
    // depth_stencil_state.depthCompareOp = vk::CompareOp::eEqual;
    // depth_stencil_state.minDepthBounds = 0.0f;
    // depth_stencil_state.maxDepthBounds = 1.0f;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto color_blend_state = vk::PipelineColorBlendStateCreateInfo{};
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &attachment;
    auto dynamic_states =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamic_state = vk::PipelineDynamicStateCreateInfo{};
    dynamic_state.dynamicStateCount =
        static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();
    auto info = vk::GraphicsPipelineCreateInfo{};
    info.stageCount = static_cast<uint32_t>(stages.size());
    info.pStages = stages.data();
    info.pVertexInputState = &vertex_input_state;
    info.pInputAssemblyState = &input_assembly_state;
    info.pViewportState = &viewport_state;
    info.pRasterizationState = &rasterization_state;
    info.pMultisampleState = &multisample_state;
    // info.pDepthStencilState = &depth_stencil_state;
    info.pColorBlendState = &color_blend_state;
    info.pDynamicState = &dynamic_state;
    info.layout = *atmosphere_pipeline_layout_;
    info.renderPass = *atmosphere_pass_;
    info.subpass = 0;
    info.basePipelineHandle = vk::Pipeline{};
    info.basePipelineIndex = -1;
    return context_->device().createGraphicsPipelineUnique({}, info).value;
  }

  vk::UniqueDescriptorPool renderer::create_descriptor_pool() {
    auto pool_sizes = std::vector<vk::DescriptorPoolSize>{
        // atmosphere
        {vk::DescriptorType::eCombinedImageSampler, 2}};
    auto info = vk::DescriptorPoolCreateInfo{};
    info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    info.maxSets = 1;
    info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    info.pPoolSizes = pool_sizes.data();
    return context_->device().createDescriptorPoolUnique(info);
  }

  vk::UniqueDescriptorSet renderer::allocate_atmosphere_descriptor_set() {
    auto info = vk::DescriptorSetAllocateInfo{};
    info.descriptorPool = *descriptor_pool_;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &*atmosphere_descriptor_set_layout_;
    auto sets = context_->device().allocateDescriptorSetsUnique(info);
    return std::move(sets[0]);
  }

  vk::Sampler renderer::create_lut_sampler() {
    auto info = vk::SamplerCreateInfo{};
    info.magFilter = vk::Filter::eLinear;
    info.minFilter = vk::Filter::eLinear;
    info.mipmapMode = vk::SamplerMipmapMode::eNearest;
    info.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    info.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    info.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->create_sampler(info);
  }

  std::vector<renderer::frame> renderer::create_frames() {
    auto device = context_->device();
    auto frames = std::vector<frame>(3);
    for (auto i = size_t{}; i < frames.size(); ++i) {
      frames[i].image_acquisition_semaphore = device.createSemaphoreUnique({});
      frames[i].queue_submission_semaphore = device.createSemaphoreUnique({});
      frames[i].queue_submission_fence =
          device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
      auto pool_info = vk::CommandPoolCreateInfo{};
      pool_info.flags = vk::CommandPoolCreateFlagBits::eTransient;
      pool_info.queueFamilyIndex = context_->graphics_family();
      frames[i].command_pool = device.createCommandPoolUnique(pool_info);
      auto buffer_info = vk::CommandBufferAllocateInfo{};
      buffer_info.commandPool = *frames[i].command_pool;
      buffer_info.level = vk::CommandBufferLevel::ePrimary;
      buffer_info.commandBufferCount = 1;
      auto command_buffers = device.allocateCommandBuffersUnique(buffer_info);
      frames[i].command_buffer = std::move(command_buffers[0]);
    }
    return frames;
  }

  void renderer::update_atmosphere_descriptor_set() {
    auto scattering_lut_info = vk::DescriptorImageInfo{};
    scattering_lut_info.sampler = lut_sampler_;
    scattering_lut_info.imageView = atmosphere_.scattering().image_view();
    scattering_lut_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto scattering_lut_write = vk::WriteDescriptorSet{};
    scattering_lut_write.dstSet = *atmosphere_descriptor_set_;
    scattering_lut_write.dstBinding = 0;
    scattering_lut_write.dstArrayElement = 0;
    scattering_lut_write.descriptorCount = 1;
    scattering_lut_write.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    scattering_lut_write.pImageInfo = &scattering_lut_info;
    auto optical_depth_lut_info = vk::DescriptorImageInfo{};
    optical_depth_lut_info.sampler = lut_sampler_;
    optical_depth_lut_info.imageView = atmosphere_.optical_depth().image_view();
    optical_depth_lut_info.imageLayout =
        vk::ImageLayout::eShaderReadOnlyOptimal;
    auto optical_depth_lut_write = vk::WriteDescriptorSet{};
    optical_depth_lut_write.dstSet = *atmosphere_descriptor_set_;
    optical_depth_lut_write.dstBinding = 1;
    optical_depth_lut_write.dstArrayElement = 0;
    optical_depth_lut_write.descriptorCount = 1;
    optical_depth_lut_write.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    optical_depth_lut_write.pImageInfo = &optical_depth_lut_info;
    auto writes = std::array{scattering_lut_write, optical_depth_lut_write};
    context_->device().updateDescriptorSets(writes, {});
  }

  void renderer::render() {
    auto logical_device = context_->device();
    auto &frame = frames_[frame_ % frames_.size()];
    auto image_acquisition_semaphore = *frame.image_acquisition_semaphore;
    auto queue_submission_semaphore = *frame.queue_submission_semaphore;
    auto queue_submission_fence = *frame.queue_submission_fence;
    auto command_pool = *frame.command_pool;
    auto command_buffer = *frame.command_buffer;
    auto framebuffer =
        window_->acquire_framebuffer(image_acquisition_semaphore, {});
    auto buffer_info = vk::CommandBufferBeginInfo{};
    buffer_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    auto clear_values = std::array<vk::ClearValue, 1>{};
    clear_values[0].color.float32 = std::array{0.0f, 0.0f, 0.0f, 0.0f};
    auto pass_info = vk::RenderPassBeginInfo{};
    pass_info.renderPass = *atmosphere_pass_;
    pass_info.framebuffer = framebuffer;
    pass_info.renderArea.offset.x = 0;
    pass_info.renderArea.offset.y = 0;
    pass_info.renderArea.extent.width = window_->swapchain_size()[0];
    pass_info.renderArea.extent.height = window_->swapchain_size()[1];
    pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    pass_info.pClearValues = clear_values.data();
    logical_device.waitForFences(
        queue_submission_fence, true, std::numeric_limits<uint64_t>::max());
    logical_device.resetFences(queue_submission_fence);
    logical_device.resetCommandPool(command_pool);
    command_buffer.begin(buffer_info);
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);
    command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, *atmosphere_pipeline_);
    auto viewport = vk::Viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = window_->swapchain_size()[0];
    viewport.height = window_->swapchain_size()[1];
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    auto scissor = vk::Rect2D{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = window_->swapchain_size()[0];
    scissor.extent.height = window_->swapchain_size()[1];
    command_buffer.setViewport(0, viewport);
    command_buffer.setScissor(0, scissor);
    command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        *atmosphere_pipeline_layout_,
        0,
        *atmosphere_descriptor_set_,
        {});
    auto x = viewport.width > viewport.height
                 ? 1.0f
                 : viewport.width / viewport.height;
    auto y = viewport.height > viewport.width
                 ? 1.0f
                 : viewport.height / viewport.width;
    auto eye_position = make_vector(0.0f, 2.0f, 0.0f);
    auto frustum_corners = std::array{
        make_vector(-x, y, -1.0f, 0.0f),
        make_vector(x, y, -1.0f, 0.0f),
        make_vector(-x, -y, -1.0f, 0.0f),
        make_vector(x, -y, -1.0f, 0.0f)};
    auto eye_orientation = rotation_matrix4x4(
        rotation_quaternion(0.0f, make_vector(1.0f, 0.0f, 0.0f)));
    for (auto &corner : frustum_corners) {
      corner = eye_orientation * corner;
      corner += concatenate(eye_position, 0.0f);
    }
    auto sun_radiance = make_vector(20.0f, 20.0f, 20.0f);
    auto delta = frame_ / 120000.0f + 1.4f * 3.141592f;
    auto sun_direction =
        normalize(make_vector(0.0f, std::cosf(delta), std::sinf(delta)));
    auto g = 0.76f;
    auto planet_radius = 6360e3f;
    auto atmosphere_radius = 6420e3f;
    auto push_constants = std::array<char, 120>{};
    std::memcpy(push_constants.data() + 0, &frustum_corners, 64);
    std::memcpy(push_constants.data() + 64, &eye_position, 12);
    std::memcpy(push_constants.data() + 80, &sun_radiance, 12);
    std::memcpy(push_constants.data() + 96, &sun_direction, 12);
    std::memcpy(push_constants.data() + 108, &g, 4);
    std::memcpy(push_constants.data() + 112, &planet_radius, 4);
    std::memcpy(push_constants.data() + 116, &atmosphere_radius, 4);
    command_buffer.pushConstants(
        *atmosphere_pipeline_layout_,
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0,
        static_cast<uint32_t>(push_constants.size()),
        push_constants.data());
    command_buffer.draw(3, 1, 0, 0);
    command_buffer.endRenderPass();
    command_buffer.end();
    auto submit_wait_semaphores = std::array{image_acquisition_semaphore};
    auto submit_wait_stages = std::array<vk::PipelineStageFlags, 1>{
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    auto submit_command_buffers = std::array{command_buffer};
    auto submit_signal_semaphores = std::array{queue_submission_semaphore};
    auto submit_info = vk::SubmitInfo{};
    submit_info.waitSemaphoreCount =
        static_cast<uint32_t>(submit_wait_semaphores.size());
    submit_info.pWaitSemaphores = submit_wait_semaphores.data();
    submit_info.pWaitDstStageMask = submit_wait_stages.data();
    submit_info.commandBufferCount =
        static_cast<uint32_t>(submit_command_buffers.size());
    submit_info.pCommandBuffers = submit_command_buffers.data();
    submit_info.signalSemaphoreCount =
        static_cast<uint32_t>(submit_signal_semaphores.size());
    submit_info.pSignalSemaphores = submit_signal_semaphores.data();
    context_->graphics_queue().submit(submit_info, queue_submission_fence);
    window_->present_framebuffer(
        static_cast<uint32_t>(submit_signal_semaphores.size()),
        submit_signal_semaphores.data(),
        framebuffer);
    ++frame_;
  }
} // namespace imp