#include "SceneView.h"

#include <fstream>
#include <iostream>

#include "../system/GpuContext.h"
#include "../util/Align.h"
#include "Scene.h"

namespace imp {
  SceneView::Flyweight::Flyweight(
      gsl::not_null<GpuContext *> context, std::size_t frameCount):
      context_{context},
      frameCount_{frameCount},
      skyViewDescriptorSetLayout_{createSkyViewDescriptorSetLayout()},
      renderDescriptorSetLayout_{createRenderDescriptorSetLayout()},
      skyViewPipelineLayout_{createSkyViewPipelineLayout()},
      renderPipelineLayout_{createRenderPipelineLayout()},
      skyViewPipeline_{createSkyViewPipeline()},
      renderPipeline_{createRenderPipeline()},
      transmittanceSampler_{createTransmittanceSampler()},
      skyViewSampler_{createSkyViewSampler()} {}

  vk::DescriptorSetLayout
  SceneView::Flyweight::createSkyViewDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 4>{};
    bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[1].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[3].descriptorType = vk::DescriptorType::eStorageImage;
    for (auto &binding : bindings) {
      binding.descriptorCount = 1;
      binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    }
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createRenderDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 5>{};
    bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[1].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[3].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[4].descriptorType = vk::DescriptorType::eStorageImage;
    for (auto &binding : bindings) {
      binding.descriptorCount = 1;
      binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    }
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createSkyViewPipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&skyViewDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createRenderPipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&renderDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::Pipeline SceneView::Flyweight::createSkyViewPipeline() const {
    auto device = context_->getDevice();
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./data/SkyView.spv", std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto code = std::vector<char>{};
    code.resize(ifs.tellg());
    if (code.size() % 4 != 0) {
      throw std::runtime_error{"invalid shader module"};
    }
    ifs.seekg(0, std::ios::beg);
    ifs.read(code.data(), code.size());
    ifs.close();
    auto moduleCreateInfo = vk::ShaderModuleCreateInfo{};
    moduleCreateInfo.codeSize = code.size();
    moduleCreateInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
    auto module = device.createShaderModuleUnique(moduleCreateInfo);
    auto createInfo = vk::ComputePipelineCreateInfo{};
    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    createInfo.stage.module = *module;
    createInfo.stage.pName = "main";
    createInfo.layout = skyViewPipelineLayout_;
    createInfo.basePipelineIndex = -1;
    return device.createComputePipeline({}, createInfo).value;
  }

  vk::Pipeline SceneView::Flyweight::createRenderPipeline() const {
    auto device = context_->getDevice();
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./data/Render.spv", std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto code = std::vector<char>{};
    code.resize(ifs.tellg());
    if (code.size() % 4 != 0) {
      throw std::runtime_error{"invalid shader module"};
    }
    ifs.seekg(0, std::ios::beg);
    ifs.read(code.data(), code.size());
    ifs.close();
    auto moduleCreateInfo = vk::ShaderModuleCreateInfo{};
    moduleCreateInfo.codeSize = code.size();
    moduleCreateInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
    auto module = device.createShaderModuleUnique(moduleCreateInfo);
    auto createInfo = vk::ComputePipelineCreateInfo{};
    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    createInfo.stage.module = *module;
    createInfo.stage.pName = "main";
    createInfo.layout = renderPipelineLayout_;
    createInfo.basePipelineIndex = -1;
    return device.createComputePipeline({}, createInfo).value;
  }

  vk::Sampler SceneView::Flyweight::createTransmittanceSampler() const {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  vk::Sampler SceneView::Flyweight::createSkyViewSampler() const {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  SceneView::Flyweight::~Flyweight() {
    auto device = context_->getDevice();
    device.destroyPipeline(renderPipeline_);
    device.destroyPipeline(skyViewPipeline_);
  }

  gsl::not_null<GpuContext *>
  SceneView::Flyweight::getContext() const noexcept {
    return context_;
  }

  std::size_t SceneView::Flyweight::getFrameCount() const noexcept {
    return frameCount_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getSkyViewDescriptorSetLayout() const noexcept {
    return skyViewDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getRenderDescriptorSetLayout() const noexcept {
    return renderDescriptorSetLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getSkyViewPipelineLayout() const noexcept {
    return skyViewPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getRenderPipelineLayout() const noexcept {
    return renderPipelineLayout_;
  }

  vk::Pipeline SceneView::Flyweight::getSkyViewPipeline() const noexcept {
    return skyViewPipeline_;
  }

  vk::Pipeline SceneView::Flyweight::getRenderPipeline() const noexcept {
    return renderPipeline_;
  }

  vk::Sampler SceneView::Flyweight::getTransmittanceSampler() const noexcept {
    return transmittanceSampler_;
  }

  vk::Sampler SceneView::Flyweight::getSkyViewSampler() const noexcept {
    return skyViewSampler_;
  }

  SceneView::Frame::Frame(GpuImage &&renderImage):
      renderImage{std::move(renderImage)} {}

  SceneView::SceneView(
      gsl::not_null<Flyweight const *> flyweight,
      gsl::not_null<std::shared_ptr<Scene>> scene,
      Extent2u const &extent) noexcept:
      flyweight_{flyweight},
      scene_{std::move(scene)},
      extent_{extent},
      uniformBuffer_{createUniformBuffer()},
      skyViewImage_{createSkyViewImage()},
      descriptorPool_{createDescriptorPool()},
      frames_{createFrames()},
      viewMatrix_{Eigen::Matrix4f::Identity()},
      invViewMatrix_{Eigen::Matrix4f::Identity()},
      projectionMatrix_{Eigen::Matrix4f::Identity()},
      invProjectionMatrix_{Eigen::Matrix4f::Identity()} {
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      initSkyViewImageView(i);
      initRenderImageView(i);
      initSkyViewDescriptorSet(i);
      initRenderDescriptorSet(i);
      initCommandPool(i);
      initCommandBuffers(i);
      initSemaphores(i);
    }
  }

  GpuBuffer SceneView::createUniformBuffer() const {
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

  GpuImage SceneView::createSkyViewImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = SKY_VIEW_IMAGE_EXTENT;
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

  vk::DescriptorPool SceneView::createDescriptorPool() const {
    auto frameCount32 = static_cast<std::uint32_t>(flyweight_->getFrameCount());
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
        {vk::DescriptorType::eUniformBuffer, 4 * frameCount32},
        {vk::DescriptorType::eCombinedImageSampler, 4 * frameCount32},
        {vk::DescriptorType::eStorageImage, 2 * frameCount32}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = 2 * frameCount32;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    return flyweight_->getContext()->getDevice().createDescriptorPool(
        createInfo);
  }

  std::vector<SceneView::Frame> SceneView::createFrames() const {
    auto frames = std::vector<SceneView::Frame>{};
    frames.reserve(flyweight_->getFrameCount());
    for (auto i = std::size_t{}; i < frames.capacity(); ++i) {
      frames.emplace_back(createRenderImage());
    }
    return frames;
  }

  GpuImage SceneView::createRenderImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width, extent_.height, 1};
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage =
        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  void SceneView::initSkyViewImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = skyViewImage_.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = skyViewImage_.getFormat();
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].skyViewImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].renderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initSkyViewDescriptorSet(std::size_t i) {
    auto setLayout = flyweight_->getSkyViewDescriptorSetLayout();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &setLayout;
    flyweight_->getContext()->getDevice().allocateDescriptorSets(
        &allocateInfo, &frames_[i].skyViewDescriptorSet);
    auto sceneViewBufferInfo = vk::DescriptorBufferInfo{};
    sceneViewBufferInfo.buffer = uniformBuffer_.get();
    sceneViewBufferInfo.offset = UNIFORM_BUFFER_STRIDE * i;
    sceneViewBufferInfo.range = UNIFORM_BUFFER_SIZE;
    auto skyViewImageInfo = vk::DescriptorImageInfo{};
    skyViewImageInfo.imageView = getSkyViewImageView(i);
    skyViewImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto writes = std::array<vk::WriteDescriptorSet, 2>{};
    writes[0].dstSet = frames_[i].skyViewDescriptorSet;
    writes[0].dstBinding = 1;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &sceneViewBufferInfo;
    writes[1].dstSet = frames_[i].skyViewDescriptorSet;
    writes[1].dstBinding = 3;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = vk::DescriptorType::eStorageImage;
    writes[1].pImageInfo = &skyViewImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(writes, {});
  }

  void SceneView::initRenderDescriptorSet(std::size_t i) {
    auto setLayout = flyweight_->getRenderDescriptorSetLayout();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &setLayout;
    flyweight_->getContext()->getDevice().allocateDescriptorSets(
        &allocateInfo, &frames_[i].renderDescriptorSet);
    auto sceneViewBufferInfo = vk::DescriptorBufferInfo{};
    sceneViewBufferInfo.buffer = uniformBuffer_.get();
    sceneViewBufferInfo.offset = UNIFORM_BUFFER_STRIDE * i;
    sceneViewBufferInfo.range = UNIFORM_BUFFER_SIZE;
    auto skyViewTextureInfo = vk::DescriptorImageInfo{};
    skyViewTextureInfo.sampler = flyweight_->getSkyViewSampler();
    skyViewTextureInfo.imageView = frames_[i].skyViewImageView;
    skyViewTextureInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto renderImageInfo = vk::DescriptorImageInfo{};
    renderImageInfo.imageView = frames_[i].renderImageView;
    renderImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto writes = std::array<vk::WriteDescriptorSet, 3>{};
    writes[0].dstSet = frames_[i].renderDescriptorSet;
    writes[0].dstBinding = 1;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &sceneViewBufferInfo;
    writes[1].dstSet = frames_[i].renderDescriptorSet;
    writes[1].dstBinding = 3;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writes[1].pImageInfo = &skyViewTextureInfo;
    writes[2].dstSet = frames_[i].renderDescriptorSet;
    writes[2].dstBinding = 4;
    writes[2].dstArrayElement = 0;
    writes[2].descriptorCount = 1;
    writes[2].descriptorType = vk::DescriptorType::eStorageImage;
    writes[2].pImageInfo = &renderImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(writes, {});
  }

  void SceneView::initCommandPool(std::size_t i) {
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    createInfo.queueFamilyIndex = flyweight_->getContext()->getComputeFamily();
    frames_[i].commandPool =
        flyweight_->getContext()->getDevice().createCommandPool(createInfo);
  }

  void SceneView::initCommandBuffers(std::size_t i) {
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandPool = frames_[i].commandPool;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = 1;
    auto device = flyweight_->getContext()->getDevice();
    device.allocateCommandBuffers(
        &allocateInfo, &frames_[i].skyViewCommandBuffer);
    device.allocateCommandBuffers(
        &allocateInfo, &frames_[i].renderCommandBuffer);
  }

  void SceneView::initSemaphores(std::size_t i) {
    auto device = flyweight_->getContext()->getDevice();
    frames_[i].skyViewSemaphore = device.createSemaphore({});
    frames_[i].renderSemaphore = device.createSemaphore({});
  }

  SceneView::~SceneView() {
    auto device = flyweight_->getContext()->getDevice();
    for (auto &frame : frames_) {
      device.destroySemaphore(frame.renderSemaphore);
      device.destroySemaphore(frame.skyViewSemaphore);
      device.destroyCommandPool(frame.commandPool);
      device.destroyImageView(frame.renderImageView);
      device.destroyImageView(frame.skyViewImageView);
    }
    device.destroyDescriptorPool(descriptorPool_);
  }

  void SceneView::render(std::size_t i) {
    auto &context = *flyweight_->getContext();
    auto queue = context.getComputeQueue();
    auto device = context.getDevice();
    auto &frame = frames_[i];
    updateUniformBuffer(i);
    if (frame.renderImage.getExtent() !=
        Extent3u{extent_.width, extent_.height, 1}) {
      updateRenderImage(i);
    }
    if (frame.scene != scene_) {
      updateSkyViewDescriptorSet(i);
      updateRenderDescriptorSet(i);
      frame.scene = scene_;
    }
    device.resetCommandPool(frame.commandPool);
    submitSkyViewCommandBuffer(i);
    submitRenderCommandBuffer(i);
  }

  void SceneView::updateRenderImage(std::size_t i) {
    auto device = flyweight_->getContext()->getDevice();
    device.destroyImageView(frames_[i].renderImageView);
    frames_[i].renderImage = createRenderImage();
    initRenderImageView(i);
  }

  void SceneView::updateUniformBuffer(std::size_t frameIndex) {
    auto viewRays = std::array{
        Eigen::Vector4f{-1.0f, 1.0f, 1.0f, 1.0f},
        Eigen::Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Eigen::Vector4f{-1.0f, -1.0f, 1.0f, 1.0f},
        Eigen::Vector4f{1.0f, -1.0f, 1.0f, 1.0f}};
    auto rayMatrix = Eigen::Matrix4f::Zero().eval();
    rayMatrix.col(0) = invViewMatrix_.col(0);
    rayMatrix.col(1) = invViewMatrix_.col(1);
    rayMatrix.col(2) = invViewMatrix_.col(2);
    rayMatrix(3, 3) = 1.0f;
    rayMatrix *= invProjectionMatrix_;
    for (auto &viewRay : viewRays) {
      viewRay = rayMatrix * viewRay;
      viewRay *= 1.0f / viewRay.w();
      viewRay.head<3>().normalize();
    }
    auto viewPosition = invViewMatrix_.col(3).head<3>().eval();
    auto skyViewNormal =
        (viewPosition - scene_->getPlanet()->getPosition()).normalized();
    auto skyViewTangent =
        (std::abs(skyViewNormal.x()) > std::abs(skyViewNormal.y())
             ? Eigen::Vector3f{-skyViewNormal.z(), 0.0f, skyViewNormal.x()} /
                   std::sqrt(
                       skyViewNormal.x() * skyViewNormal.x() +
                       skyViewNormal.z() * skyViewNormal.z())
             : Eigen::Vector3f{0.0f, skyViewNormal.z(), -skyViewNormal.y()} /
                   std::sqrt(
                       skyViewNormal.y() * skyViewNormal.y() +
                       skyViewNormal.z() * skyViewNormal.z()))
            .eval();
    auto skyViewBitangent = skyViewNormal.cross(skyViewTangent);
    auto offset = UNIFORM_BUFFER_STRIDE * frameIndex;
    auto data = uniformBuffer_.getMappedData() + offset;
    std::memcpy(data + 0, &viewPosition, 12);
    std::memcpy(data + 16, &viewRays[0], 12);
    std::memcpy(data + 32, &viewRays[1], 12);
    std::memcpy(data + 48, &viewRays[2], 12);
    std::memcpy(data + 64, &viewRays[3], 12);
    std::memcpy(data + 80, &skyViewTangent, 12);
    std::memcpy(data + 96, &skyViewBitangent, 12);
    uniformBuffer_.flush(offset, UNIFORM_BUFFER_SIZE);
  }

  void SceneView::updateSkyViewDescriptorSet(std::size_t i) {
    auto sceneBufferInfo = vk::DescriptorBufferInfo{};
    sceneBufferInfo.buffer = scene_->getUniformBuffer().get();
    sceneBufferInfo.offset = Scene::UNIFORM_BUFFER_STRIDE * i;
    sceneBufferInfo.range = Scene::UNIFORM_BUFFER_SIZE;
    auto transmittanceTextureInfo = vk::DescriptorImageInfo{};
    transmittanceTextureInfo.sampler = flyweight_->getTransmittanceSampler();
    transmittanceTextureInfo.imageView = scene_->getTransmittanceImageView(i);
    transmittanceTextureInfo.imageLayout =
        vk::ImageLayout::eShaderReadOnlyOptimal;
    auto writes = std::array<vk::WriteDescriptorSet, 2>{};
    writes[0].dstSet = frames_[i].skyViewDescriptorSet;
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &sceneBufferInfo;
    writes[1].dstSet = frames_[i].skyViewDescriptorSet;
    writes[1].dstBinding = 2;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writes[1].pImageInfo = &transmittanceTextureInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(writes, {});
  }

  void SceneView::updateRenderDescriptorSet(std::size_t i) {
    auto sceneBufferInfo = vk::DescriptorBufferInfo{};
    sceneBufferInfo.buffer = scene_->getUniformBuffer().get();
    sceneBufferInfo.offset = Scene::UNIFORM_BUFFER_STRIDE * i;
    sceneBufferInfo.range = Scene::UNIFORM_BUFFER_SIZE;
    auto sceneBufferWrite = vk::WriteDescriptorSet{};
    sceneBufferWrite.dstSet = frames_[i].renderDescriptorSet;
    sceneBufferWrite.dstBinding = 0;
    sceneBufferWrite.dstArrayElement = 0;
    sceneBufferWrite.descriptorCount = 1;
    sceneBufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    sceneBufferWrite.pBufferInfo = &sceneBufferInfo;
    auto transmittanceLutInfo = vk::DescriptorImageInfo{};
    transmittanceLutInfo.sampler = flyweight_->getTransmittanceSampler();
    transmittanceLutInfo.imageView = scene_->getTransmittanceImageView(i);
    transmittanceLutInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto transmittanceLutWrite = vk::WriteDescriptorSet{};
    transmittanceLutWrite.dstSet = frames_[i].renderDescriptorSet;
    transmittanceLutWrite.dstBinding = 2;
    transmittanceLutWrite.dstArrayElement = 0;
    transmittanceLutWrite.descriptorCount = 1;
    transmittanceLutWrite.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    transmittanceLutWrite.pImageInfo = &transmittanceLutInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {sceneBufferWrite, transmittanceLutWrite}, {});
  }

  void SceneView::submitSkyViewCommandBuffer(std::size_t i) {
    auto &frame = frames_[i];
    frame.skyViewCommandBuffer.begin(
        {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    frame.skyViewCommandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getSkyViewPipeline());
    frame.skyViewCommandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getSkyViewPipelineLayout(),
        0,
        frame.skyViewDescriptorSet,
        {});
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = skyViewImage_.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
      barrier.subresourceRange.layerCount = 1;
      frame.skyViewCommandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.skyViewCommandBuffer.dispatch(
        skyViewImage_.getExtent().width / 8,
        skyViewImage_.getExtent().height / 8,
        1);
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = {};
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = skyViewImage_.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
      barrier.subresourceRange.layerCount = 1;
      frame.skyViewCommandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eBottomOfPipe,
          {},
          {},
          {},
          barrier);
    }
    frame.skyViewCommandBuffer.end();
    auto waitSemaphore = scene_->getSemaphore(i);
    auto waitStage =
        vk::PipelineStageFlags{vk::PipelineStageFlagBits::eComputeShader};
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.skyViewCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frame.skyViewSemaphore;
    flyweight_->getContext()->getComputeQueue().submit(submitInfo);
  }

  void SceneView::submitRenderCommandBuffer(std::size_t i) {
    auto &frame = frames_[i];
    frame.renderCommandBuffer.begin(
        {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    frame.renderCommandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getRenderPipeline());
    frame.renderCommandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getRenderPipelineLayout(),
        0,
        frame.renderDescriptorSet,
        {});
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.renderCommandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.renderCommandBuffer.dispatch(
        extent_.width / 8, extent_.height / 8, 1);
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = {};
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex =
          flyweight_->getContext()->getComputeFamily();
      barrier.dstQueueFamilyIndex =
          flyweight_->getContext()->getGraphicsFamily();
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.renderCommandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eBottomOfPipe,
          {},
          {},
          {},
          barrier);
    }
    frame.renderCommandBuffer.end();
    auto waitStage =
        vk::PipelineStageFlags{vk::PipelineStageFlagBits::eComputeShader};
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &frame.skyViewSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.renderCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frame.renderSemaphore;
    flyweight_->getContext()->getComputeQueue().submit(submitInfo);
  }

  gsl::not_null<SceneView::Flyweight const *>
  SceneView::getFlyweight() const noexcept {
    return flyweight_;
  }

  gsl::not_null<std::shared_ptr<Scene>> SceneView::getScene() const noexcept {
    return scene_;
  }

  void
  SceneView::setScene(gsl::not_null<std::shared_ptr<Scene>> scene) noexcept {
    scene_ = std::move(scene);
  }

  Extent2u const &SceneView::getExtent() const noexcept {
    return extent_;
  }

  void SceneView::setExtent(Extent2u const &extent) noexcept {
    extent_ = extent;
  }

  GpuBuffer const &SceneView::getUniformBuffer() const noexcept {
    return uniformBuffer_;
  }

  GpuImage const &SceneView::getSkyViewImage() const noexcept {
    return skyViewImage_;
  }

  GpuImage const &SceneView::getRenderImage(std::size_t i) const noexcept {
    return frames_[i].renderImage;
  }

  vk::ImageView SceneView::getSkyViewImageView(std::size_t i) const noexcept {
    return frames_[i].skyViewImageView;
  }

  vk::ImageView SceneView::getRenderImageView(std::size_t i) const noexcept {
    return frames_[i].renderImageView;
  }

  vk::Semaphore SceneView::getSkyViewSemaphore(std::size_t i) const noexcept {
    return frames_[i].skyViewSemaphore;
  }

  vk::Semaphore SceneView::getRenderSemaphore(std::size_t i) const noexcept {
    return frames_[i].renderSemaphore;
  }

  Eigen::Matrix4f const &SceneView::getViewMatrix() const noexcept {
    return viewMatrix_;
  }

  void SceneView::setViewMatrix(Eigen::Matrix4f const &m) noexcept {
    viewMatrix_ = m;
    invViewMatrix_ = m.inverse();
  }

  Eigen::Matrix4f const &SceneView::getProjectionMatrix() const noexcept {
    return projectionMatrix_;
  }

  void SceneView::setProjectionMatrix(Eigen::Matrix4f const &m) noexcept {
    projectionMatrix_ = m;
    invProjectionMatrix_ = m.inverse();
  }
} // namespace imp