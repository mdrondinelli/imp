#include "Frame.h"

#include <fstream>

#include "../system/Display.h"
#include "Camera.h"
#include "Scene.h"

namespace imp {
  Frame::Flyweight::Flyweight(
      Display *display,
      TransmittanceLut::Flyweight const *transmittanceLutFlyweight,
      SkyViewLut::Flyweight const *skyViewLutFlyweight):
      display_{display},
      transmittanceLutFlyweight_{transmittanceLutFlyweight},
      skyViewLutFlyweight_{skyViewLutFlyweight},
      renderPass_{createRenderPass()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()} {}

  Display *Frame::Flyweight::getDisplay() const noexcept {
    return display_;
  }

  TransmittanceLut::Flyweight const *
  Frame::Flyweight::getTransmittanceLutFlyweight() const noexcept {
    return transmittanceLutFlyweight_;
  }

  SkyViewLut::Flyweight const *
  Frame::Flyweight::getSkyViewLutFlyweight() const noexcept {
    return skyViewLutFlyweight_;
  }

  vk::RenderPass Frame::Flyweight::getRenderPass() const noexcept {
    return *renderPass_;
  }

  vk::PipelineLayout Frame::Flyweight::getPipelineLayout() const noexcept {
    return *pipelineLayout_;
  }

  vk::Pipeline Frame::Flyweight::getPipeline() const noexcept {
    return *pipeline_;
  }

  vk::UniqueRenderPass Frame::Flyweight::createRenderPass() {
    auto attachment = vk::AttachmentDescription{};
    attachment.format = display_->getSurfaceFormat().format;
    attachment.samples = vk::SampleCountFlagBits::e1;
    attachment.loadOp = vk::AttachmentLoadOp::eClear;
    attachment.storeOp = vk::AttachmentStoreOp::eStore;
    attachment.initialLayout = vk::ImageLayout::eUndefined;
    attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    auto subpassColorAttachment = vk::AttachmentReference{};
    subpassColorAttachment.attachment = 0;
    subpassColorAttachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = vk::SubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &subpassColorAttachment;
    auto createInfo = vk::RenderPassCreateInfo{};
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    return display_->getContext()->getDevice().createRenderPassUnique(
        createInfo);
  }

  vk::UniquePipelineLayout Frame::Flyweight::createPipelineLayout() {
    auto setLayouts = std::array{
        transmittanceLutFlyweight_->getRenderDescriptorSetLayout(),
        skyViewLutFlyweight_->getRenderDescriptorSetLayout()};
    auto pushConstantRange = vk::PushConstantRange{};
    pushConstantRange.stageFlags =
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 84;
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = static_cast<std::uint32_t>(setLayouts.size());
    createInfo.pSetLayouts = setLayouts.data();
    createInfo.pushConstantRangeCount = 1;
    createInfo.pPushConstantRanges = &pushConstantRange;
    return display_->getContext()->getDevice().createPipelineLayoutUnique(
        createInfo);
  }

  vk::UniquePipeline Frame::Flyweight::createPipeline() {
    auto context = display_->getContext();
    auto createShaderModule = [=](char const *path) {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open(path, std::ios::binary);
      in.seekg(0, std::ios::end);
      auto codeSize = static_cast<std::size_t>(in.tellg());
      if (codeSize % 4 != 0) {
        throw std::runtime_error{"invalid shader module"};
      }
      code.resize(codeSize);
      in.seekg(0, std::ios::beg);
      in.read(code.data(), codeSize);
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = codeSize;
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      return context->getDevice().createShaderModuleUnique(createInfo);
    };
    auto vertModule = createShaderModule("./res/AtmosphereVert.spv");
    auto vertStage = vk::PipelineShaderStageCreateInfo{};
    vertStage.stage = vk::ShaderStageFlagBits::eVertex;
    vertStage.module = *vertModule;
    vertStage.pName = "main";
    auto fragModule = createShaderModule("./res/AtmosphereFrag.spv");
    auto fragStage = vk::PipelineShaderStageCreateInfo{};
    fragStage.stage = vk::ShaderStageFlagBits::eFragment;
    fragStage.module = *fragModule;
    fragStage.pName = "main";
    auto stages = std::array{vertStage, fragStage};
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.polygonMode = vk::PolygonMode::eFill;
    rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
    rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    // auto depth_stencil_state = vk::PipelineDepthStencilStateCreateInfo{};
    // depth_stencil_state.depthTestEnable = false;
    // depth_stencil_state.depthCompareOp = vk::CompareOp::eEqual;
    // depth_stencil_state.minDepthBounds = 0.0f;
    // depth_stencil_state.maxDepthBounds = 1.0f;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &attachment;
    auto dynamicStates =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
    dynamicState.dynamicStateCount =
        static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizationState;
    createInfo.pMultisampleState = &multisampleState;
    // info.pDepthStencilState = &depth_stencil_state;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = *pipelineLayout_;
    createInfo.renderPass = *renderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = vk::Pipeline{};
    createInfo.basePipelineIndex = -1;
    return context->getDevice()
        .createGraphicsPipelineUnique({}, createInfo)
        .value;
  }

  Frame::Frame(
      Flyweight const *flyweight,
      unsigned transmittanceLutWidth,
      unsigned transmittanceLutHeight,
      unsigned skyViewLutWidth,
      unsigned skyViewLutHeight):
      flyweight_{flyweight},
      imageAcquisitionSemaphore_{flyweight_->getDisplay()
                                     ->getContext()
                                     ->getDevice()
                                     .createSemaphoreUnique({})},
      queueSubmissionSemaphore_{flyweight_->getDisplay()
                                    ->getContext()
                                    ->getDevice()
                                    .createSemaphoreUnique({})},
      queueSubmissionFence_{
          flyweight_->getDisplay()->getContext()->getDevice().createFenceUnique(
              {vk::FenceCreateFlagBits::eSignaled})},
      commandPool_{createCommandPool()},
      commandBuffer_{allocateCommandBuffer()},
      atmosphereBuffer_{flyweight_->getDisplay()->getContext()->getAllocator()},
      transmittanceLut_{
          flyweight_->getTransmittanceLutFlyweight(),
          &atmosphereBuffer_,
          transmittanceLutWidth,
          transmittanceLutHeight},
      skyViewLut_{
          flyweight_->getSkyViewLutFlyweight(),
          &transmittanceLut_,
          skyViewLutWidth,
          skyViewLutHeight} {}

  void
  Frame::render(Scene const &scene, Camera const &camera, std::uint32_t seed) {
    atmosphereBuffer_.update(*scene.getAtmosphere());
    auto &window = *flyweight_->getDisplay();
    auto &context = *window.getContext();
    auto framebuffer =
        window.acquireFramebuffer(*imageAcquisitionSemaphore_, {});
    auto beginInfo = vk::CommandBufferBeginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    auto waitStage = vk::PipelineStageFlags{
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &*imageAcquisitionSemaphore_;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &*commandBuffer_;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &*queueSubmissionSemaphore_;
    context.getDevice().waitForFences(
        *queueSubmissionFence_,
        true,
        std::numeric_limits<std::uint64_t>::max());
    context.getDevice().resetFences(*queueSubmissionFence_);
    context.getDevice().resetCommandPool(*commandPool_);
    commandBuffer_->begin(beginInfo);
    computeTransmittanceLut(*scene.getAtmosphere());
    computeSkyViewLut(*scene.getSunLight(), camera);
    renderAtmosphere(camera, seed, framebuffer);
    commandBuffer_->end();
    context.getGraphicsQueue().submit(submitInfo, *queueSubmissionFence_);
    window.presentFramebuffer(1, &*queueSubmissionSemaphore_, framebuffer);
  }

  namespace {
    vk::ImageMemoryBarrier createLayoutTransition(
        vk::AccessFlags srcAccessMask,
        vk::AccessFlags dstAccessMask,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::Image image) {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = srcAccessMask;
      barrier.dstAccessMask = dstAccessMask;
      barrier.oldLayout = oldLayout;
      barrier.newLayout = newLayout;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = image;
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      return barrier;
    }
  } // namespace

  void Frame::computeTransmittanceLut(Atmosphere const &atmosphere) {
    if (transmittanceLut_.compute(*commandBuffer_, atmosphere)) {
      commandBuffer_->pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader |
              vk::PipelineStageFlagBits::eFragmentShader,
          {},
          {},
          {},
          {createLayoutTransition(
              vk::AccessFlagBits::eShaderWrite,
              vk::AccessFlagBits::eShaderRead,
              vk::ImageLayout::eGeneral,
              vk::ImageLayout::eShaderReadOnlyOptimal,
              transmittanceLut_.getImage())});
    }
  }

  void
  Frame::computeSkyViewLut(DirectionalLight const &sun, Camera const &camera) {
    skyViewLut_.compute(*commandBuffer_, sun, camera);
    commandBuffer_->pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        {},
        {},
        {createLayoutTransition(
            vk::AccessFlagBits::eShaderWrite,
            vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eGeneral,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            skyViewLut_.getImage())});
  }

  void Frame::renderAtmosphere(
      Camera const &camera, std::uint32_t seed, vk::Framebuffer framebuffer) {
    auto &window = *flyweight_->getDisplay();
    auto clearValue = vk::ClearValue{};
    clearValue.color.float32 = std::array{0.0f, 0.0f, 0.0f, 0.0f};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.framebuffer = framebuffer;
    renderPassBegin.renderArea.offset.x = 0;
    renderPassBegin.renderArea.offset.y = 0;
    renderPassBegin.renderArea.extent.width = window.getSwapchainWidth();
    renderPassBegin.renderArea.extent.height = window.getSwapchainHeight();
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    auto viewport = vk::Viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = window.getSwapchainWidth();
    viewport.height = window.getSwapchainHeight();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    auto scissor = vk::Rect2D{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = window.getSwapchainWidth();
    scissor.extent.height = window.getSwapchainHeight();
    auto invView = getViewMatrix(camera).inverse().eval();
    auto invProjection = getProjectionMatrix(camera).inverse().eval();
    auto frustumCorners = std::array{
        Eigen::Vector4f{-1.0f, -1.0f, 1.0f, 1.0f},
        Eigen::Vector4f{1.0f, -1.0f, 1.0f, 1.0f},
        Eigen::Vector4f{-1.0f, 1.0f, 1.0f, 1.0f},
        Eigen::Vector4f{1.0f, 1.0f, 1.0f, 1.0f}};
    for (auto &frustumCorner : frustumCorners) {
      frustumCorner = invProjection * frustumCorner;
      frustumCorner /= frustumCorner[3];
      frustumCorner = invView * frustumCorner;
    }
    auto pushConstants = std::array<char, 80>{};
    std::memcpy(pushConstants.data() + 0, &frustumCorners, 64);
    std::memcpy(pushConstants.data() + 64, &camera.getPosition(), 12);
    std::memcpy(pushConstants.data() + 76, &seed, 4);
    commandBuffer_->beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    commandBuffer_->bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getPipeline());
    commandBuffer_->setViewport(0, viewport);
    commandBuffer_->setScissor(0, scissor);
    commandBuffer_->bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getPipelineLayout(),
        0,
        {transmittanceLut_.getRenderDescriptorSet(),
         skyViewLut_.getRenderDescriptorSet()},
        {});
    commandBuffer_->pushConstants(
        flyweight_->getPipelineLayout(),
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0,
        84,
        pushConstants.data());
    commandBuffer_->draw(3, 1, 0, 0);
    commandBuffer_->endRenderPass();
  }

  vk::UniqueCommandPool Frame::createCommandPool() {
    auto &context = *flyweight_->getDisplay()->getContext();
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    createInfo.queueFamilyIndex = context.getGraphicsFamily();
    return context.getDevice().createCommandPoolUnique(createInfo);
  }

  vk::UniqueCommandBuffer Frame::allocateCommandBuffer() {
    auto &context = *flyweight_->getDisplay()->getContext();
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandPool = *commandPool_;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = 1;
    return std::move(
        context.getDevice().allocateCommandBuffersUnique(allocateInfo)[0]);
  }

  GpuBuffer Frame::createAtmosphereBuffer() {
    auto buffer = vk::BufferCreateInfo{};
    buffer.size = 68;
    buffer.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocation.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    return {
        flyweight_->getDisplay()->getContext()->getAllocator(),
        buffer,
        allocation};
  }
} // namespace imp