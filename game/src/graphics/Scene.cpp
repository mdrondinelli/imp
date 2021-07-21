#include "Scene.h"

#include <fstream>

#include "../system/GpuContext.h"

namespace imp {
  Scene::Flyweight::Flyweight(
      gsl::not_null<GpuContext *> context, std::size_t frameCount):
      context_{context},
      frameCount_{frameCount},
      transmittanceDescriptorSetLayout_{
          createTransmittanceDescriptorSetLayout()},
      transmittancePipelineLayout_{createTransmittancePipelineLayout()},
      transmittancePipeline_{createTransmittancePipeline()} {}

  vk::DescriptorSetLayout
  Scene::Flyweight::createTransmittanceDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 2>{};
    bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = vk::ShaderStageFlagBits::eCompute;
    bindings[1].descriptorType = vk::DescriptorType::eStorageImage;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::PipelineLayout
  Scene::Flyweight::createTransmittancePipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = gsl::span{&transmittanceDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::Pipeline Scene::Flyweight::createTransmittancePipeline() const {
    auto code = std::vector<char>{};
    auto in = std::ifstream{};
    in.exceptions(std::ios::badbit | std::ios::failbit);
    in.open("./data/Transmittance.spv", std::ios::binary);
    in.seekg(0, std::ios::end);
    auto codeSize = static_cast<std::size_t>(in.tellg());
    if (codeSize % 4 != 0) {
      throw std::runtime_error{"invalid shader module"};
    }
    code.resize(codeSize);
    in.seekg(0, std::ios::beg);
    in.read(code.data(), code.size());
    auto moduleCreateInfo = vk::ShaderModuleCreateInfo{};
    moduleCreateInfo.codeSize = code.size();
    moduleCreateInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
    auto module =
        context_->getDevice().createShaderModuleUnique(moduleCreateInfo);
    auto pipelineCreateInfo = vk::ComputePipelineCreateInfo{};
    pipelineCreateInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipelineCreateInfo.stage.module = *module;
    pipelineCreateInfo.stage.pName = "main";
    pipelineCreateInfo.layout = transmittancePipelineLayout_;
    pipelineCreateInfo.basePipelineIndex = -1;
    return context_->getDevice()
        .createComputePipeline({}, pipelineCreateInfo)
        .value;
  }

  Scene::Flyweight::~Flyweight() {
    auto device = context_->getDevice();
    device.destroyPipeline(transmittancePipeline_);
  }

  gsl::not_null<GpuContext *> Scene::Flyweight::getContext() const noexcept {
    return context_;
  }

  std::size_t Scene::Flyweight::getFrameCount() const noexcept {
    return frameCount_;
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

  Scene::Scene(gsl::not_null<Flyweight const *> flyweight):
      flyweight_{flyweight},
      uniformBuffer_{createUniformBuffer()},
      transmittanceImage_{createTransmittanceImage()},
      descriptorPool_{createDescriptorPool()},
      commandPool_{createCommandPool()},
      frames_(flyweight_->getFrameCount()) {
    initTransmittanceImageViews();
    initTransmittaceDescriptorSets();
    initCommandBuffers();
    initSemaphores();
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

  GpuImage Scene::createTransmittanceImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = TRANSMITTANCE_IMAGE_EXTENT;
    image.mipLevels = 1;
    image.arrayLayers = static_cast<std::uint32_t>(flyweight_->getFrameCount());
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
    image.sharingMode = vk::SharingMode::eExclusive;
    image.initialLayout = vk::ImageLayout::eUndefined;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  vk::DescriptorPool Scene::createDescriptorPool() const {
    auto frameCount32 = static_cast<std::uint32_t>(flyweight_->getFrameCount());
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
        {vk::DescriptorType::eUniformBuffer, frameCount32},
        {vk::DescriptorType::eStorageImage, frameCount32}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = frameCount32;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    return flyweight_->getContext()->getDevice().createDescriptorPool(
        createInfo);
  }

  vk::CommandPool Scene::createCommandPool() const {
    auto &context = *flyweight_->getContext();
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.queueFamilyIndex = context.getComputeFamily();
    return context.getDevice().createCommandPool(createInfo);
  }

  void Scene::initTransmittanceImageViews() {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = transmittanceImage_.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = transmittanceImage_.getFormat();
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      createInfo.subresourceRange.baseArrayLayer =
          static_cast<std::uint32_t>(i);
      frames_[i].transmittanceImageView =
          flyweight_->getContext()->getDevice().createImageView(createInfo);
    }
  }

  void Scene::initTransmittaceDescriptorSets() {
    auto setLayout = flyweight_->getTransmittanceDescriptorSetLayout();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &setLayout;
    auto writes = std::array<vk::WriteDescriptorSet, 2>{};
    auto sceneBufferInfo = vk::DescriptorBufferInfo{};
    sceneBufferInfo.buffer = uniformBuffer_.get();
    sceneBufferInfo.range = UNIFORM_BUFFER_SIZE;
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &sceneBufferInfo;
    auto transmittanceImageInfo = vk::DescriptorImageInfo{};
    transmittanceImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    writes[1].dstBinding = 1;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = vk::DescriptorType::eStorageImage;
    writes[1].pImageInfo = &transmittanceImageInfo;
    auto device = flyweight_->getContext()->getDevice();
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      device.allocateDescriptorSets(
          &allocateInfo, &frames_[i].transmittanceDescriptorSet);
      sceneBufferInfo.offset = UNIFORM_BUFFER_STRIDE * i;
      writes[0].dstSet = frames_[i].transmittanceDescriptorSet;
      transmittanceImageInfo.imageView = frames_[i].transmittanceImageView;
      writes[1].dstSet = frames_[i].transmittanceDescriptorSet;
      device.updateDescriptorSets(writes, {});
    }
  }

  void Scene::initCommandBuffers() {
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandPool = commandPool_;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = 1;
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      flyweight_->getContext()->getDevice().allocateCommandBuffers(
          &allocateInfo, &frames_[i].commandBuffer);
      frames_[i].commandBuffer.begin(vk::CommandBufferBeginInfo{});
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.oldLayout = vk::ImageLayout::eUndefined;
        barrier.newLayout = vk::ImageLayout::eGeneral;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = transmittanceImage_.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
        barrier.subresourceRange.layerCount = 1;
        frames_[i].commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      frames_[i].commandBuffer.bindPipeline(
          vk::PipelineBindPoint::eCompute,
          flyweight_->getTransmittancePipeline());
      frames_[i].commandBuffer.bindDescriptorSets(
          vk::PipelineBindPoint::eCompute,
          flyweight_->getTransmittancePipelineLayout(),
          0,
          frames_[i].transmittanceDescriptorSet,
          {});
      frames_[i].commandBuffer.dispatch(
          TRANSMITTANCE_IMAGE_EXTENT.width / 8,
          TRANSMITTANCE_IMAGE_EXTENT.height / 8,
          1);
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.dstAccessMask = {};
        barrier.oldLayout = vk::ImageLayout::eGeneral;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = transmittanceImage_.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
        barrier.subresourceRange.layerCount = 1;
        frames_[i].commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eComputeShader,
            vk::PipelineStageFlagBits::eBottomOfPipe,
            // vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      frames_[i].commandBuffer.end();
    }
  }

  void Scene::initSemaphores() {
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      frames_[i].semaphore =
          flyweight_->getContext()->getDevice().createSemaphore({});
    }
  }

  Scene::~Scene() {
    auto device = flyweight_->getContext()->getDevice();
    for (auto &frame : frames_) {
      device.destroySemaphore(frame.semaphore);
      device.destroyImageView(frame.transmittanceImageView);
    }
    device.destroyCommandPool(commandPool_);
    device.destroyDescriptorPool(descriptorPool_);
  }

  void Scene::render(std::size_t frameIndex) {
    updateUniformBuffer(frameIndex);
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frames_[frameIndex].commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frames_[frameIndex].semaphore;
    flyweight_->getContext()->getComputeQueue().submit(submitInfo);
  }

  void Scene::updateUniformBuffer(std::size_t frameIndex) {
    auto offset = UNIFORM_BUFFER_STRIDE * frameIndex;
    auto data = uniformBuffer_.getMappedData() + offset;
    planet_->store(data);
    data += align(DirectionalLight::UNIFORM_ALIGN, Planet::UNIFORM_SIZE);
    sunLight_->store(data);
    // data += align(..., DirectionalLight::UNIFORM_SIZE);
    uniformBuffer_.flush(offset, UNIFORM_BUFFER_SIZE);
  }

  gsl::not_null<Scene::Flyweight const *> Scene::getFlyweight() const noexcept {
    return flyweight_;
  }

  GpuBuffer const &Scene::getUniformBuffer() const noexcept {
    return uniformBuffer_;
  }

  GpuImage const &Scene::getTransmittanceImage() const noexcept {
    return transmittanceImage_;
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