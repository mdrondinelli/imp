#include "Scene.h"

#include <fstream>

#include "../system/GpuContext.h"
#include "../util/Math.h"

namespace imp {
  Scene::Flyweight::Flyweight(
      gsl::not_null<GpuContext *> context, std::size_t frameCount):
      context_{context},
      frameCount_{frameCount},
      transmittanceRenderPass_{createTransmittanceRenderPass()},
      transmittanceDescriptorSetLayout_{
          createTransmittanceDescriptorSetLayout()},
      transmittancePipelineLayout_{createTransmittancePipelineLayout()},
      transmittancePipeline_{createTransmittancePipeline()},
      transmittanceSampler_{createTransmittanceSampler()} {}

  vk::RenderPass Scene::Flyweight::createTransmittanceRenderPass() const {
    auto attachmentDesc = GpuAttachmentDescription{};
    attachmentDesc.format = vk::Format::eR16G16B16A16Sfloat;
    attachmentDesc.samples = vk::SampleCountFlagBits::e1;
    attachmentDesc.loadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    attachmentDesc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto attachmentRef = GpuAttachmentReference{};
    attachmentRef.attachment = 0;
    attachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = GpuSubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachments = {&attachmentRef, 1};
    auto dependency = GpuSubpassDependency{};
    dependency.srcSubpass = 0;
    dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependency.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    auto createInfo = GpuRenderPassCreateInfo{};
    createInfo.attachments = {&attachmentDesc, 1};
    createInfo.subpasses = {&subpass, 1};
    createInfo.dependencies = {&dependency, 1};
    return context_->createRenderPass(createInfo);
  }

  vk::DescriptorSetLayout
  Scene::Flyweight::createTransmittanceDescriptorSetLayout() const {
    auto binding = GpuDescriptorSetLayoutBinding{};
    binding.descriptorType = vk::DescriptorType::eUniformBuffer;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = {&binding, 1};
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::PipelineLayout
  Scene::Flyweight::createTransmittancePipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = gsl::span{&transmittanceDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::Pipeline Scene::Flyweight::createTransmittancePipeline() const {
    auto vertModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/GenericVert.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      vertModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto fragModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/TransmittanceFrag.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      fragModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto stages = std::array<vk::PipelineShaderStageCreateInfo, 2>{};
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = *vertModule;
    stages[0].pName = "main";
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = *fragModule;
    stages[1].pName = "main";
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
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
    createInfo.layout = transmittancePipelineLayout_;
    createInfo.renderPass = transmittanceRenderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineIndex = -1;
    return context_->getDevice().createGraphicsPipeline({}, createInfo).value;
  }

  vk::Sampler Scene::Flyweight::createTransmittanceSampler() const {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  Scene::Flyweight::~Flyweight() {
    context_->getDevice().destroy(transmittancePipeline_);
  }

  gsl::not_null<GpuContext *> Scene::Flyweight::getContext() const noexcept {
    return context_;
  }

  std::size_t Scene::Flyweight::getFrameCount() const noexcept {
    return frameCount_;
  }

  vk::RenderPass Scene::Flyweight::getTransmittanceRenderPass() const noexcept {
    return transmittanceRenderPass_;
  }

  vk::DescriptorSetLayout
  Scene::Flyweight::getTransmittanceDescriptorSetLayout() const noexcept {
    return transmittanceDescriptorSetLayout_;
  }

  vk::PipelineLayout
  Scene::Flyweight::getTransmittancePipelineLayout() const noexcept {
    return transmittancePipelineLayout_;
  }

  vk::Pipeline Scene::Flyweight::getTransmittancePipeline() const noexcept {
    return transmittancePipeline_;
  }

  vk::Sampler Scene::Flyweight::getTransmittanceSampler() const noexcept {
    return transmittanceSampler_;
  }

  Scene::Frame::Frame(GpuImage &&transmittanceImage) noexcept:
      transmittanceImage{std::move(transmittanceImage)} {}

  Scene::Scene(gsl::not_null<Flyweight const *> flyweight):
      flyweight_{flyweight},
      descriptorPool_{createDescriptorPool()},
      uniformBuffer_{createUniformBuffer()},
      frames_{createFrames()},
      firstFrame_{true} {}

  vk::DescriptorPool Scene::createDescriptorPool() const {
    auto frameCount32 = static_cast<std::uint32_t>(flyweight_->getFrameCount());
    auto poolSize = vk::DescriptorPoolSize{
        vk::DescriptorType::eUniformBuffer, frameCount32};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = frameCount32;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    return flyweight_->getContext()->getDevice().createDescriptorPool(
        createInfo);
  }

  GpuBuffer Scene::createUniformBuffer() const {
    auto frameCount = flyweight_->getFrameCount();
    auto buffer = vk::BufferCreateInfo{};
    buffer.size = frameCount != 1 ? UNIFORM_BUFFER_STRIDE * frameCount
                                  : UNIFORM_BUFFER_SIZE;
    buffer.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocation.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    return GpuBuffer{
        flyweight_->getContext()->getAllocator(), buffer, allocation};
  }

  std::vector<Scene::Frame> Scene::createFrames() const {
    auto frames = std::vector<Frame>{};
    frames.reserve(flyweight_->getFrameCount());
    for (auto i = std::size_t{}; i < frames.capacity(); ++i) {
      frames.emplace_back(createTransmittanceImage());
      initTransmittanceImageView(frames.back());
      initTransmittanceFramebuffer(frames.back());
      initTransmittanceDescriptorSet(frames.back());
      updateTransmittanceDescriptorSet(frames.back(), i);
      initCommandPool(frames.back());
      initCommandBuffer(frames.back());
      updateCommandBuffer(frames.back());
      initSemaphore(frames.back());
    }
    return frames;
  }

  GpuImage Scene::createTransmittanceImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = TRANSMITTANCE_IMAGE_EXTENT;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage = vk::ImageUsageFlagBits::eSampled |
                  vk::ImageUsageFlagBits::eColorAttachment;
    image.sharingMode = vk::SharingMode::eExclusive;
    image.initialLayout = vk::ImageLayout::eUndefined;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  void Scene::initTransmittanceImageView(Frame &frame) const {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.transmittanceImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.transmittanceImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    frame.transmittanceImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void Scene::initTransmittanceFramebuffer(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getTransmittanceRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &frame.transmittanceImageView;
    createInfo.width = frame.transmittanceImage.getExtent().width;
    createInfo.height = frame.transmittanceImage.getExtent().height;
    createInfo.layers = 1;
    frame.transmittanceFramebuffer =
        flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
  }

  void Scene::initTransmittanceDescriptorSet(Frame &frame) const {
    auto setLayout = flyweight_->getTransmittanceDescriptorSetLayout();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &setLayout;
    flyweight_->getContext()->getDevice().allocateDescriptorSets(
        &allocateInfo, &frame.transmittanceDescriptorSet);
  }

  void Scene::updateTransmittanceDescriptorSet(
      Frame &frame, std::size_t index) const {
    auto info = vk::DescriptorBufferInfo{};
    info.buffer = uniformBuffer_.get();
    info.offset = UNIFORM_BUFFER_STRIDE * index;
    info.range = UNIFORM_BUFFER_SIZE;
    auto write = vk::WriteDescriptorSet{};
    write.dstSet = frame.transmittanceDescriptorSet;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eUniformBuffer;
    write.pBufferInfo = &info;
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
  }

  void Scene::initCommandPool(Frame &frame) const {
    auto &context = *flyweight_->getContext();
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.queueFamilyIndex = context.getGraphicsFamily();
    frame.commandPool = context.getDevice().createCommandPool(createInfo);
  }

  void Scene::initCommandBuffer(Frame &frame) const {
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandPool = frame.commandPool;
    allocateInfo.commandBufferCount = 1;
    flyweight_->getContext()->getDevice().allocateCommandBuffers(
        &allocateInfo, &frame.commandBuffer);
  }

  void Scene::updateCommandBuffer(Frame &frame) const {
    auto commandBufferBegin = vk::CommandBufferBeginInfo{};
    frame.commandBuffer.begin(commandBufferBegin);
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getTransmittanceRenderPass();
    renderPassBegin.framebuffer = frame.transmittanceFramebuffer;
    renderPassBegin.renderArea.extent.width =
        frame.transmittanceImage.getExtent().width;
    renderPassBegin.renderArea.extent.height =
        frame.transmittanceImage.getExtent().height;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getTransmittancePipeline());
    auto viewport = vk::Viewport{};
    viewport.width = renderPassBegin.renderArea.extent.width;
    viewport.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setViewport(0, viewport);
    auto scissor = vk::Rect2D{};
    scissor.extent.width = renderPassBegin.renderArea.extent.width;
    scissor.extent.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getTransmittancePipelineLayout(),
        0,
        frame.transmittanceDescriptorSet,
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();
    frame.commandBuffer.end();
  }

  void Scene::initSemaphore(Frame &frame) const {
    frame.semaphore = flyweight_->getContext()->getDevice().createSemaphore({});
  }

  Scene::~Scene() {
    auto device = flyweight_->getContext()->getDevice();
    for (auto &frame : frames_) {
      device.destroy(frame.semaphore);
      device.destroy(frame.commandPool);
      device.destroy(frame.transmittanceFramebuffer);
      device.destroy(frame.transmittanceImageView);
    }
    device.destroyDescriptorPool(descriptorPool_);
  }

  void Scene::render(std::size_t i) {
    auto &frame = frames_[i];
    updateUniformBuffer(i);
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.commandBuffer;
    /*submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frame.semaphore;*/
    flyweight_->getContext()->getGraphicsQueue().submit(submitInfo);
    firstFrame_ = false;
  }

  void Scene::updateUniformBuffer(std::size_t frameIndex) {
    auto offset = UNIFORM_BUFFER_STRIDE * frameIndex;
    auto data = uniformBuffer_.getMappedData() + offset;
    planet_->store(data);
    data += align(DirectionalLight::UNIFORM_ALIGN, Planet::UNIFORM_SIZE);
    sunLight_->store(data);
    uniformBuffer_.flush(offset, UNIFORM_BUFFER_SIZE);
  }

  gsl::not_null<Scene::Flyweight const *> Scene::getFlyweight() const noexcept {
    return flyweight_;
  }

  GpuBuffer const &Scene::getUniformBuffer() const noexcept {
    return uniformBuffer_;
  }

  GpuImage const &Scene::getTransmittanceImage(std::size_t i) const noexcept {
    return frames_[i].transmittanceImage;
  }

  vk::ImageView Scene::getTransmittanceImageView(std::size_t i) const noexcept {
    return frames_[i].transmittanceImageView;
  }

  vk::Semaphore Scene::getSemaphore(std::size_t i) const noexcept {
    return frames_[i].semaphore;
  }

  std::shared_ptr<Planet> Scene::getPlanet() const noexcept {
    return planet_;
  }

  void Scene::setPlanet(std::shared_ptr<Planet> planet) noexcept {
    planet_ = std::move(planet);
  }

  std::shared_ptr<DirectionalLight> Scene::getSunLight() const noexcept {
    return sunLight_;
  }

  void Scene::setSunLight(std::shared_ptr<DirectionalLight> light) noexcept {
    sunLight_ = std::move(light);
  }

  std::shared_ptr<DirectionalLight> Scene::getMoonLight() const noexcept {
    return moonLight_;
  }

  void Scene::setMoonLight(std::shared_ptr<DirectionalLight> light) noexcept {
    moonLight_ = std::move(light);
  }
} // namespace imp