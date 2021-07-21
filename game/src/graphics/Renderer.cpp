#include "Renderer.h"

#include <fstream>
#include <iostream>

#include "../system/Display.h"

namespace imp {
  Renderer::Renderer(gsl::not_null<Display *> window, std::size_t frameCount):
      window_{window},
      sceneFlyweight_{window->getContext(), frameCount},
      sceneViewFlyweight_{window->getContext(), frameCount},
      descriptorSetLayout_{createDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()},
      defaultRenderImage_{createDefaultRenderImage()},
      defaultRenderImageView_{createDefaultRenderImageView()},
      frames_(frameCount),
      descriptorPool_{createDescriptorPool()},
      vertexBuffer_{createVertexBuffer()},
      indexBuffer_{createIndexBuffer()},
      vertexBufferData_{nullptr},
      indexBufferData_{nullptr},
      vertexBufferIndex_{0},
      indexBufferIndex_{0},
      textureIndex_{0},
      ditherSeed_{0},
      frameIndex_{frameCount - 1} {
    initDescriptorSets();
    initCommandPools();
    initCommandBuffers();
    initSynchronization();
  }

  vk::DescriptorSetLayout Renderer::createDescriptorSetLayout() const {
    auto binding = GpuDescriptorSetLayoutBinding{};
    binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    binding.descriptorCount = TEXTURE_ARRAY_SIZE;
    binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = {&binding, 1};
    return window_->getContext()->createDescriptorSetLayout(createInfo);
  }

  vk::PipelineLayout Renderer::createPipelineLayout() const {
    auto pushConstantRange = GpuPushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.size = 4;
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&descriptorSetLayout_, 1};
    createInfo.pushConstantRanges = {&pushConstantRange, 1};
    return window_->getContext()->createPipelineLayout(createInfo);
  }

  vk::Pipeline Renderer::createPipeline() const {
    auto createModule = [this](auto path) {
      auto ifs = std::ifstream{};
      ifs.exceptions(std::ios::badbit | std::ios::failbit);
      ifs.open(path, std::ios::binary);
      ifs.seekg(0, std::ios::end);
      auto code = std::vector<char>{};
      code.resize(ifs.tellg());
      ifs.seekg(0, std::ios::beg);
      ifs.read(code.data(), code.size());
      ifs.close();
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      return window_->getContext()->getDevice().createShaderModuleUnique(
          createInfo);
    };
    auto vertModule = createModule("./data/CompositeVert.spv");
    auto fragModule = createModule("./data/CompositeFrag.spv");
    auto stages = std::array{
        vk::PipelineShaderStageCreateInfo{
            {}, vk::ShaderStageFlagBits::eVertex, *vertModule, "main"},
        vk::PipelineShaderStageCreateInfo{
            {}, vk::ShaderStageFlagBits::eFragment, *fragModule, "main"}};
    auto vertexBindingDescription = vk::VertexInputBindingDescription{};
    vertexBindingDescription.binding = 0;
    vertexBindingDescription.stride = 16;
    vertexBindingDescription.inputRate = vk::VertexInputRate::eVertex;
    auto vertexAttributionDescriptions = std::array{
        vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32Sfloat, 0},
        vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32Uint, 8},
        vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32Uint, 12}};
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    vertexInputState.vertexBindingDescriptionCount = 1;
    vertexInputState.pVertexBindingDescriptions = &vertexBindingDescription;
    vertexInputState.vertexAttributeDescriptionCount =
        static_cast<std::uint32_t>(vertexAttributionDescriptions.size());
    vertexInputState.pVertexAttributeDescriptions =
        vertexAttributionDescriptions.data();
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
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
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = pipelineLayout_;
    createInfo.renderPass = window_->getRenderPass();
    createInfo.basePipelineIndex = -1;
    return window_->getContext()
        ->getDevice()
        .createGraphicsPipeline({}, createInfo)
        .value;
  }

  vk::Sampler Renderer::createSampler() const {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return window_->getContext()->createSampler(createInfo);
  }

  GpuImage Renderer::createDefaultRenderImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent.width = 8;
    image.extent.height = 8;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.usage = vk::ImageUsageFlagBits::eSampled;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{window_->getContext()->getAllocator(), image, allocation};
  }

  vk::ImageView Renderer::createDefaultRenderImageView() const {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = defaultRenderImage_.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = defaultRenderImage_.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    return window_->getContext()->getDevice().createImageView(createInfo);
  }

  vk::DescriptorPool Renderer::createDescriptorPool() const {
    auto frameCount = static_cast<std::uint32_t>(frames_.size());
    auto poolSize = vk::DescriptorPoolSize{};
    poolSize.type = vk::DescriptorType::eCombinedImageSampler;
    poolSize.descriptorCount = TEXTURE_ARRAY_SIZE * frameCount;
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = frameCount;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    return window_->getContext()->getDevice().createDescriptorPool(createInfo);
  }

  GpuBuffer Renderer::createVertexBuffer() const {
    auto buffer = vk::BufferCreateInfo{};
    buffer.size = VERTEX_BUFFER_SIZE * frames_.size();
    buffer.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocation.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    return GpuBuffer{
        window_->getContext()->getAllocator(),
        buffer,
        allocation,
        "Renderer::vertexBuffer_"};
  }

  GpuBuffer Renderer::createIndexBuffer() const {
    auto buffer = vk::BufferCreateInfo{};
    buffer.size = INDEX_BUFFER_SIZE * frames_.size();
    buffer.usage = vk::BufferUsageFlagBits::eIndexBuffer;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocation.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    return GpuBuffer{
        window_->getContext()->getAllocator(),
        buffer,
        allocation,
        "Renderer::indexBuffer_"};
  }

  void Renderer::initDescriptorSets() {
    auto device = window_->getContext()->getDevice();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &descriptorSetLayout_;
    for (auto &frame : frames_) {
      device.allocateDescriptorSets(&allocateInfo, &frame.descriptorSet);
      auto info = vk::DescriptorImageInfo{};
      info.sampler = sampler_;
      info.imageView = defaultRenderImageView_;
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto write = vk::WriteDescriptorSet{};
      write.dstSet = frame.descriptorSet;
      write.dstBinding = 0;
      write.descriptorCount = 1;
      write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo = &info;
      for (auto i = std::uint32_t{}; i < TEXTURE_ARRAY_SIZE; ++i) {
        write.dstArrayElement = i;
        device.updateDescriptorSets(write, {});
      }
    }
  }

  void Renderer::initCommandPools() {
    auto device = window_->getContext()->getDevice();
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    createInfo.queueFamilyIndex = window_->getContext()->getGraphicsFamily();
    for (auto &frame : frames_) {
      frame.commandPool = device.createCommandPool(createInfo);
    }
  }

  void Renderer::initCommandBuffers() {
    auto device = window_->getContext()->getDevice();
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandBufferCount = 1;
    for (auto &frame : frames_) {
      allocateInfo.commandPool = frame.commandPool;
      device.allocateCommandBuffers(&allocateInfo, &frame.commandBuffer);
    }
  }

  void Renderer::initSynchronization() {
    auto device = window_->getContext()->getDevice();
    for (auto &frame : frames_) {
      frame.swapchainSemaphore = device.createSemaphore({});
      frame.frameSemaphore = device.createSemaphore({});
      frame.frameFence =
          device.createFence({vk::FenceCreateFlagBits::eSignaled});
    }
  }

  Renderer::~Renderer() {
    auto device = window_->getContext()->getDevice();
    // TODO: remove
    device.waitIdle();
    device.destroy(descriptorPool_);
    for (auto &frame : frames_) {
      device.destroy(frame.frameFence);
      device.destroy(frame.frameSemaphore);
      device.destroy(frame.swapchainSemaphore);
      device.destroy(frame.commandPool);
    }
    device.destroy(defaultRenderImageView_);
    device.destroyPipeline(pipeline_);
    // device.destroyRenderPass(renderPass_);
  }

  void Renderer::begin() {
    if (++frameIndex_ == frames_.size()) {
      frameIndex_ = 0;
    }
    auto device = window_->getContext()->getDevice();
    auto &frame = frames_[frameIndex_];
    device.waitForFences(
        frame.frameFence, false, std::numeric_limits<std::uint64_t>::max());
    device.resetFences(frame.frameFence);
    device.resetCommandPool(frame.commandPool);
    scenes_.clear();
    sceneViews_.clear();
    vertexBufferData_ = reinterpret_cast<Vertex *>(
        vertexBuffer_.getMappedData() + VERTEX_BUFFER_SIZE * frameIndex_);
    indexBufferData_ = reinterpret_cast<std::uint16_t *>(
        indexBuffer_.getMappedData() + INDEX_BUFFER_SIZE * frameIndex_);
    vertexBufferIndex_ = 0;
    indexBufferIndex_ = 0;
    textureIndex_ = 0;
  }

  void Renderer::end() {
    vertexBuffer_.flush(
        VERTEX_BUFFER_SIZE * frameIndex_, sizeof(Vertex) * vertexBufferIndex_);
    indexBuffer_.flush(
        INDEX_BUFFER_SIZE * frameIndex_,
        sizeof(std::uint16_t) * indexBufferIndex_);
    auto &context = *window_->getContext();
    auto &frame = frames_[frameIndex_];
    auto imageIndex = window_->acquireImage(frame.swapchainSemaphore, {});
    frame.commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    {
      auto barriers = std::vector<vk::ImageMemoryBarrier>();
      if (context.getComputeFamily() != context.getGraphicsFamily()) {
        barriers.reserve(sceneViews_.size() + 1);
        for (auto &[sceneView, _] : sceneViews_) {
          auto &barrier = barriers.emplace_back();
          barrier.srcAccessMask = {};
          barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
          barrier.oldLayout = vk::ImageLayout::eGeneral;
          barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
          barrier.srcQueueFamilyIndex = context.getComputeFamily();
          barrier.dstQueueFamilyIndex = context.getGraphicsFamily();
          barrier.image = sceneView->getRenderImage(frameIndex_).get();
          barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
          barrier.subresourceRange.baseMipLevel = 0;
          barrier.subresourceRange.levelCount = 1;
          barrier.subresourceRange.baseArrayLayer = 0;
          barrier.subresourceRange.layerCount = 1;
        }
      }
      auto &barrier = barriers.emplace_back();
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = defaultRenderImage_.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eFragmentShader,
          {},
          {},
          {},
          barriers);
    }
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = window_->getRenderPass();
    renderPassBegin.framebuffer = window_->getFramebuffer(imageIndex);
    renderPassBegin.renderArea.extent.width = window_->getSwapchainWidth();
    renderPassBegin.renderArea.extent.height = window_->getSwapchainHeight();
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, pipeline_);
    frame.commandBuffer.pushConstants(
        pipelineLayout_,
        vk::ShaderStageFlagBits::eFragment,
        0,
        4,
        &ditherSeed_);
    ++ditherSeed_;
    auto viewport = vk::Viewport{};
    viewport.width = window_->getSwapchainWidth();
    viewport.height = window_->getSwapchainHeight();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    frame.commandBuffer.setViewport(0, viewport);
    auto scissor = vk::Rect2D{};
    scissor.extent.width = window_->getSwapchainWidth();
    scissor.extent.height = window_->getSwapchainHeight();
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        pipelineLayout_,
        0,
        frame.descriptorSet,
        {});
    frame.commandBuffer.bindVertexBuffers(
        0, vertexBuffer_.get(), VERTEX_BUFFER_SIZE * frameIndex_);
    frame.commandBuffer.bindIndexBuffer(
        indexBuffer_.get(),
        INDEX_BUFFER_SIZE * frameIndex_,
        vk::IndexType::eUint16);
    frame.commandBuffer.drawIndexed(indexBufferIndex_, 1, 0, 0, 0);
    frame.commandBuffer.endRenderPass();
    frame.commandBuffer.end();
    auto waitSemaphores = std::vector<vk::Semaphore>();
    auto waitStages = std::vector<vk::PipelineStageFlags>();
    waitSemaphores.reserve(sceneViews_.size() + 1);
    waitStages.reserve(sceneViews_.size() + 1);
    for (auto &[sceneView, _] : sceneViews_) {
      waitSemaphores.emplace_back(sceneView->getRenderSemaphore(frameIndex_));
      waitStages.emplace_back(vk::PipelineStageFlagBits::eFragmentShader);
    }
    waitSemaphores.emplace_back(frame.swapchainSemaphore);
    waitStages.emplace_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.commandBuffer;
    submitInfo.waitSemaphoreCount =
        static_cast<std::uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frame.frameSemaphore;
    window_->getContext()->getGraphicsQueue().submit(
        submitInfo, frame.frameFence);
    window_->present({&frame.frameSemaphore, 1}, imageIndex);
  }

  void Renderer::draw(
      gsl::not_null<std::shared_ptr<SceneView>> sceneView,
      int x,
      int y,
      int w,
      int h) {
    if (sizeof(Vertex) * (vertexBufferIndex_ + 4) > VERTEX_BUFFER_SIZE) {
      throw std::runtime_error{"max vertices reached"};
    }
    if (sizeof(std::uint16_t) * (indexBufferIndex_ + 6) > INDEX_BUFFER_SIZE) {
      throw std::runtime_error{"max indices reached"};
    }
    if (textureIndex_ == TEXTURE_ARRAY_SIZE) {
      throw std::runtime_error{"max textures reached"};
    }
    if (scenes_.emplace(sceneView->getScene()).second) {
      sceneView->getScene()->render(frameIndex_);
    }
    auto textureIndex = std::uint32_t{};
    if (sceneViews_.emplace(sceneView, textureIndex_).second) {
      textureIndex = textureIndex_++;
      sceneView->render(frameIndex_);
    } else {
      textureIndex = sceneViews_.at(sceneView);
    }
    auto left = 2.0f / window_->getSwapchainWidth() * x - 1.0f;
    auto right = 2.0f / window_->getSwapchainWidth() * (x + w) - 1.0f;
    auto top = 2.0f / window_->getSwapchainHeight() * y - 1.0f;
    auto bottom = 2.0f / window_->getSwapchainHeight() * (y + h) - 1.0f;
    vertexBufferData_[vertexBufferIndex_ + 0].position = {right, top};
    vertexBufferData_[vertexBufferIndex_ + 0].vertexIndex = 0;
    vertexBufferData_[vertexBufferIndex_ + 0].textureIndex = textureIndex;
    vertexBufferData_[vertexBufferIndex_ + 1].position = {left, top};
    vertexBufferData_[vertexBufferIndex_ + 1].vertexIndex = 1;
    vertexBufferData_[vertexBufferIndex_ + 1].textureIndex = textureIndex;
    vertexBufferData_[vertexBufferIndex_ + 2].position = {left, bottom};
    vertexBufferData_[vertexBufferIndex_ + 2].vertexIndex = 2;
    vertexBufferData_[vertexBufferIndex_ + 2].textureIndex = textureIndex;
    vertexBufferData_[vertexBufferIndex_ + 3].position = {right, bottom};
    vertexBufferData_[vertexBufferIndex_ + 3].vertexIndex = 3;
    vertexBufferData_[vertexBufferIndex_ + 3].textureIndex = textureIndex;
    indexBufferData_[indexBufferIndex_ + 0] = vertexBufferIndex_ + 0;
    indexBufferData_[indexBufferIndex_ + 1] = vertexBufferIndex_ + 1;
    indexBufferData_[indexBufferIndex_ + 2] = vertexBufferIndex_ + 2;
    indexBufferData_[indexBufferIndex_ + 3] = vertexBufferIndex_ + 2;
    indexBufferData_[indexBufferIndex_ + 4] = vertexBufferIndex_ + 3;
    indexBufferData_[indexBufferIndex_ + 5] = vertexBufferIndex_ + 0;
    auto info = vk::DescriptorImageInfo{};
    info.sampler = sampler_;
    info.imageView = sceneView->getRenderImageView(frameIndex_);
    info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto write = vk::WriteDescriptorSet{};
    write.dstSet = frames_[frameIndex_].descriptorSet;
    write.dstBinding = 0;
    write.dstArrayElement = textureIndex;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.pImageInfo = &info;
    window_->getContext()->getDevice().updateDescriptorSets(write, {});
    vertexBufferIndex_ += 4;
    indexBufferIndex_ += 6;
  }

  gsl::not_null<Scene::Flyweight const *>
  Renderer::getSceneFlyweight() const noexcept {
    return gsl::not_null{&sceneFlyweight_};
  }

  gsl::not_null<SceneView::Flyweight const *>
  Renderer::getSceneViewFlyweight() const noexcept {
    return gsl::not_null{&sceneViewFlyweight_};
  }

} // namespace imp