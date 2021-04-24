#include "Renderer.h"

#include <fstream>

#include "../math/Matrix.h"

namespace imp {
  namespace {
    constexpr auto const FRAME_CONCURRENCY = 3u;
  }

  Renderer::Renderer(RendererCreateInfo const &createInfo):
      context_{createInfo.context},
      window_{createInfo.window},
      atmospherePass_{createAtmospherePass()},
      atmosphereDescriptorSetLayout_{createAtmosphereDescriptorSetLayout()},
      atmospherePipelineLayout_{createAtmospherePipelineLayout()},
      atmospherePipeline_{createAtmospherePipeline()},
      descriptorPool_{createDescriptorPool()},
      // atmosphereDescriptorSet_{allocateAtmosphereDescriptorSet()},
      lutSampler_{createLutSampler()},
      frames_{createFrames()} {}

  Renderer::~Renderer() {
    context_->getDevice().waitIdle();
  }

  void Renderer::render(Scene const &scene) {
    auto device = context_->getDevice();
    auto &frame = frames_[frame_ % frames_.size()];
    auto image_acquisition_semaphore = *frame.imageAcquisitionSemaphore;
    auto queue_submission_semaphore = *frame.queueSubmissionSemaphore;
    auto queue_submission_fence = *frame.queueSubmissionFence;
    auto command_pool = *frame.commandPool;
    auto command_buffer = *frame.commandBuffer;
    auto framebuffer =
        window_->acquireFramebuffer(image_acquisition_semaphore, {});
    auto buffer_info = vk::CommandBufferBeginInfo{};
    buffer_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    auto clear_values = std::array<vk::ClearValue, 1>{};
    clear_values[0].color.float32 = std::array{0.0f, 0.0f, 0.0f, 0.0f};
    auto pass_info = vk::RenderPassBeginInfo{};
    pass_info.renderPass = *atmospherePass_;
    pass_info.framebuffer = framebuffer;
    pass_info.renderArea.offset.x = 0;
    pass_info.renderArea.offset.y = 0;
    pass_info.renderArea.extent.width = window_->getSwapchainSize()[0];
    pass_info.renderArea.extent.height = window_->getSwapchainSize()[1];
    pass_info.clearValueCount = static_cast<std::uint32_t>(clear_values.size());
    pass_info.pClearValues = clear_values.data();
    device.waitForFences(
        queue_submission_fence,
        true,
        std::numeric_limits<std::uint64_t>::max());
    device.resetFences(queue_submission_fence);
    device.resetCommandPool(command_pool);
    updateAtmosphereDescriptorSet(frame, scene.getAtmosphere());
    command_buffer.begin(buffer_info);
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);
    command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, *atmospherePipeline_);
    auto viewport = vk::Viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = window_->getSwapchainSize()[0];
    viewport.height = window_->getSwapchainSize()[1];
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    auto scissor = vk::Rect2D{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = window_->getSwapchainSize()[0];
    scissor.extent.height = window_->getSwapchainSize()[1];
    command_buffer.setViewport(0, viewport);
    command_buffer.setScissor(0, scissor);
    command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        *atmospherePipelineLayout_,
        0,
        frame.atmosphereDescriptorSet,
        {});
    auto &camera = scene.getCamera();
    auto eyePosition = camera.getTransform().getTranslation();
    auto frustumCorners = std::array{
        makeVector(-1.0f, -1.0f, 1.0f, 1.0f),
        makeVector(1.0f, -1.0f, 1.0f, 1.0f),
        makeVector(-1.0f, 1.0f, 1.0f, 1.0f),
        makeVector(1.0f, 1.0f, 1.0f, 1.0f)};
    auto invProjection = inverse(camera.getProjectionMatrix());
    auto invView = inverse(camera.getViewMatrix());
    for (auto &p : frustumCorners) {
      p = invProjection * p;
      p /= p[3];
      p += concatenate(eyePosition, 0.0f);
    }
    auto sun_radiance = makeVector(20.0f, 20.0f, 20.0f);
    auto delta = frame_ / 30.0f + 1.4f * 3.141592f;
    auto sun_direction =
        normalize(makeVector(0.0f, std::cosf(delta), std::sinf(delta)));
    auto g = 0.76f;
    auto planetRadius = scene.getAtmosphere().getPlanetRadius();
    auto atmosphereRadius = scene.getAtmosphere().getAtmosphereRadius();
    auto time = static_cast<std::uint32_t>(frame_);
    auto push_constants = std::array<char, 124>{};
    std::memcpy(push_constants.data() + 0, &frustumCorners, 64);
    std::memcpy(push_constants.data() + 64, &eyePosition, 12);
    std::memcpy(push_constants.data() + 80, &sun_radiance, 12);
    std::memcpy(push_constants.data() + 96, &sun_direction, 12);
    std::memcpy(push_constants.data() + 108, &g, 4);
    std::memcpy(push_constants.data() + 112, &planetRadius, 4);
    std::memcpy(push_constants.data() + 116, &atmosphereRadius, 4);
    std::memcpy(push_constants.data() + 120, &time, 4);
    command_buffer.pushConstants(
        *atmospherePipelineLayout_,
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0,
        static_cast<std::uint32_t>(push_constants.size()),
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
        static_cast<std::uint32_t>(submit_wait_semaphores.size());
    submit_info.pWaitSemaphores = submit_wait_semaphores.data();
    submit_info.pWaitDstStageMask = submit_wait_stages.data();
    submit_info.commandBufferCount =
        static_cast<std::uint32_t>(submit_command_buffers.size());
    submit_info.pCommandBuffers = submit_command_buffers.data();
    submit_info.signalSemaphoreCount =
        static_cast<std::uint32_t>(submit_signal_semaphores.size());
    submit_info.pSignalSemaphores = submit_signal_semaphores.data();
    context_->getGraphicsQueue().submit(submit_info, queue_submission_fence);
    window_->presentFramebuffer(
        static_cast<std::uint32_t>(submit_signal_semaphores.size()),
        submit_signal_semaphores.data(),
        framebuffer);
    ++frame_;
  }

  void Renderer::updateAtmosphereDescriptorSet(
      Frame &frame, Atmosphere const &atmosphere) {
    auto opticalDepthInfo = vk::DescriptorImageInfo{};
    opticalDepthInfo.sampler = *lutSampler_;
    opticalDepthInfo.imageView = atmosphere.getOpticalDepthLut().getImageView();
    opticalDepthInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto opticalDepthWrite = vk::WriteDescriptorSet{};
    opticalDepthWrite.dstSet = frame.atmosphereDescriptorSet;
    opticalDepthWrite.dstBinding = 0;
    opticalDepthWrite.dstArrayElement = 0;
    opticalDepthWrite.descriptorCount = 1;
    opticalDepthWrite.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    opticalDepthWrite.pImageInfo = &opticalDepthInfo;
    auto scatteringInfo = vk::DescriptorImageInfo{};
    scatteringInfo.sampler = *lutSampler_;
    scatteringInfo.imageView = atmosphere.getScatteringLut().getImageView();
    scatteringInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto scatteringWrite = vk::WriteDescriptorSet{};
    scatteringWrite.dstSet = frame.atmosphereDescriptorSet;
    scatteringWrite.dstBinding = 1;
    scatteringWrite.dstArrayElement = 0;
    scatteringWrite.descriptorCount = 1;
    scatteringWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    scatteringWrite.pImageInfo = &scatteringInfo;
    auto writes = std::array{opticalDepthWrite, scatteringWrite};
    context_->getDevice().updateDescriptorSets(writes, {});
  }

  vk::UniqueRenderPass Renderer::createAtmospherePass() {
    auto color_attachment = vk::AttachmentDescription{};
    color_attachment.format = window_->getFormat();
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
    auto info = vk::RenderPassCreateInfo{};
    info.attachmentCount = 1;
    info.pAttachments = &color_attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    return context_->getDevice().createRenderPassUnique(info);
  }

  vk::UniqueDescriptorSetLayout
  Renderer::createAtmosphereDescriptorSetLayout() {
    auto opticalDepth = vk::DescriptorSetLayoutBinding{};
    opticalDepth.binding = 0;
    opticalDepth.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    opticalDepth.descriptorCount = 1;
    opticalDepth.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto scattering = vk::DescriptorSetLayoutBinding{};
    scattering.binding = 1;
    scattering.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    scattering.descriptorCount = 1;
    scattering.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto bindings = std::array{opticalDepth, scattering};
    auto createInfo = vk::DescriptorSetLayoutCreateInfo{};
    createInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
    createInfo.pBindings = bindings.data();
    return context_->getDevice().createDescriptorSetLayoutUnique(createInfo);
  }

  namespace {
    vk::UniqueShaderModule
    createShaderModule(GpuContext &context, std::filesystem::path const &path) {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open(path, std::ios::binary);
      in.seekg(0, std::ios::end);
      auto size = static_cast<std::size_t>(in.tellg());
      if (size % 4 != 0) {
        throw std::runtime_error{"failed to create shader module"};
      }
      code.resize(size);
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto moduleCreateInfo = vk::ShaderModuleCreateInfo{};
      moduleCreateInfo.codeSize = static_cast<std::uint32_t>(code.size());
      moduleCreateInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      return context.getDevice().createShaderModuleUnique(moduleCreateInfo);
    }
  } // namespace

  vk::UniquePipelineLayout Renderer::createAtmospherePipelineLayout() {
    auto set_layouts = std::array{*atmosphereDescriptorSetLayout_};
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
    return context_->getDevice().createPipelineLayoutUnique(info);
  }

  vk::UniquePipeline Renderer::createAtmospherePipeline() {
    auto vert_module =
        createShaderModule(*context_, "./res/atmosphere-vert.spv");
    auto vert_stage = vk::PipelineShaderStageCreateInfo{};
    vert_stage.stage = vk::ShaderStageFlagBits::eVertex;
    vert_stage.module = *vert_module;
    vert_stage.pName = "main";
    auto frag_module =
        createShaderModule(*context_, "./res/atmosphere-frag.spv");
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
        static_cast<std::uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();
    auto info = vk::GraphicsPipelineCreateInfo{};
    info.stageCount = static_cast<std::uint32_t>(stages.size());
    info.pStages = stages.data();
    info.pVertexInputState = &vertex_input_state;
    info.pInputAssemblyState = &input_assembly_state;
    info.pViewportState = &viewport_state;
    info.pRasterizationState = &rasterization_state;
    info.pMultisampleState = &multisample_state;
    // info.pDepthStencilState = &depth_stencil_state;
    info.pColorBlendState = &color_blend_state;
    info.pDynamicState = &dynamic_state;
    info.layout = *atmospherePipelineLayout_;
    info.renderPass = *atmospherePass_;
    info.subpass = 0;
    info.basePipelineHandle = vk::Pipeline{};
    info.basePipelineIndex = -1;
    return context_->getDevice().createGraphicsPipelineUnique({}, info).value;
  }

  vk::UniqueDescriptorPool Renderer::createDescriptorPool() {
    auto pool_sizes = std::vector<vk::DescriptorPoolSize>{
        // Atmosphere
        {vk::DescriptorType::eCombinedImageSampler, 2 * FRAME_CONCURRENCY}};
    auto info = vk::DescriptorPoolCreateInfo{};
    // info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    info.maxSets = FRAME_CONCURRENCY;
    info.poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size());
    info.pPoolSizes = pool_sizes.data();
    return context_->getDevice().createDescriptorPoolUnique(info);
  }

  vk::UniqueSampler Renderer::createLutSampler() {
    auto info = vk::SamplerCreateInfo{};
    info.magFilter = vk::Filter::eLinear;
    info.minFilter = vk::Filter::eLinear;
    info.mipmapMode = vk::SamplerMipmapMode::eNearest;
    info.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    info.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    info.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->getDevice().createSamplerUnique(info);
  }

  std::vector<Renderer::Frame> Renderer::createFrames() {
    auto device = context_->getDevice();
    auto frames = std::vector<Frame>(FRAME_CONCURRENCY);
    for (auto i = std::size_t{}; i < frames.size(); ++i) {
      frames[i].imageAcquisitionSemaphore = device.createSemaphoreUnique({});
      frames[i].queueSubmissionSemaphore = device.createSemaphoreUnique({});
      frames[i].queueSubmissionFence =
          device.createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
      auto poolCreateInfo = vk::CommandPoolCreateInfo{};
      poolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
      poolCreateInfo.queueFamilyIndex = context_->getGraphicsFamily();
      frames[i].commandPool = device.createCommandPoolUnique(poolCreateInfo);
      auto bufferAllocateInfo = vk::CommandBufferAllocateInfo{};
      bufferAllocateInfo.commandPool = *frames[i].commandPool;
      bufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
      bufferAllocateInfo.commandBufferCount = 1;
      auto buffers = device.allocateCommandBuffersUnique(bufferAllocateInfo);
      frames[i].commandBuffer = std::move(buffers[0]);
      auto setAllocateInfo = vk::DescriptorSetAllocateInfo{};
      setAllocateInfo.descriptorPool = *descriptorPool_;
      setAllocateInfo.descriptorSetCount = 1;
      setAllocateInfo.pSetLayouts = &*atmosphereDescriptorSetLayout_;
      auto sets = device.allocateDescriptorSets(setAllocateInfo);
      frames[i].atmosphereDescriptorSet = sets[0];
    }
    return frames;
  }
} // namespace imp