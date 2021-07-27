#include "SceneView.h"

#include <fstream>
#include <iostream>

#include "../system/GpuContext.h"
#include "../util/Align.h"
#include "../util/Math.h"
#include "Scene.h"

namespace imp {
  SceneView::Flyweight::Flyweight(
      gsl::not_null<GpuContext *> context, std::size_t frameCount):
      context_{context},
      frameCount_{frameCount},
      skyViewDescriptorSetLayout_{createSkyViewDescriptorSetLayout()},
      renderDescriptorSetLayout_{createRenderDescriptorSetLayout()},
      downsampleDescriptorSetLayout_{createDownsampleDescriptorSetLayout()},
      blurDescriptorSetLayout_{createBlurDescriptorSetLayout()},
      blurBlendDescriptorSetLayout_{createBlurBlendDescriptorSetLayout()},
      bloomDescriptorSetLayout_{createBloomDescriptorSetLayout()},
      skyViewPipelineLayout_{createSkyViewPipelineLayout()},
      renderPipelineLayout_{createRenderPipelineLayout()},
      downsamplePipelineLayout_{createDownsamplePipelineLayout()},
      blurPipelineLayout_{createBlurPipelineLayout()},
      blurBlendPipelineLayout_{createBlurBlendPipelineLayout()},
      bloomPipelineLayout_{createBloomPipelineLayout()},
      skyViewPipeline_{createSkyViewPipeline()},
      renderPipeline_{createRenderPipeline()},
      downsamplePipeline_{createDownsamplePipeline()},
      blurPipelines_{createBlurPipelines()},
      blurBlendPipelines_{createBlurBlendPipelines()},
      bloomPipeline_{createBloomPipeline()},
      skyViewSampler_{createSkyViewSampler()},
      generalSampler_{createGeneralSampler()} {}

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

  vk::DescriptorSetLayout
  SceneView::Flyweight::createDownsampleDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 2>{};
    bindings[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[1].descriptorType = vk::DescriptorType::eStorageImage;
    for (auto &binding : bindings) {
      binding.descriptorCount = 1;
      binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    }
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createBlurDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 2>{};
    bindings[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[1].descriptorType = vk::DescriptorType::eStorageImage;
    for (auto &binding : bindings) {
      binding.descriptorCount = 1;
      binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    }
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createBlurBlendDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 3>{};
    bindings[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[2].descriptorType = vk::DescriptorType::eStorageImage;
    for (auto &binding : bindings) {
      binding.descriptorCount = 1;
      binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    }
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createBloomDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 2>{};
    bindings[0].descriptorType = vk::DescriptorType::eStorageImage;
    bindings[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
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

  vk::PipelineLayout
  SceneView::Flyweight::createDownsamplePipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&downsampleDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createBlurPipelineLayout() const {
    auto pushConstantRange = GpuPushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
    pushConstantRange.size = 4;
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&blurDescriptorSetLayout_, 1};
    createInfo.pushConstantRanges = {&pushConstantRange, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout
  SceneView::Flyweight::createBlurBlendPipelineLayout() const {
    auto pushConstantRange = GpuPushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
    pushConstantRange.size = 12;
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&blurBlendDescriptorSetLayout_, 1};
    createInfo.pushConstantRanges = {&pushConstantRange, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createBloomPipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&bloomDescriptorSetLayout_, 1};
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

  vk::Pipeline SceneView::Flyweight::createDownsamplePipeline() const {
    auto device = context_->getDevice();
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./data/Downsample.spv", std::ios::binary);
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
    createInfo.layout = downsamplePipelineLayout_;
    createInfo.basePipelineIndex = -1;
    return device.createComputePipeline({}, createInfo).value;
  }

  absl::flat_hash_map<int, vk::Pipeline>
  SceneView::Flyweight::createBlurPipelines() const {
    auto device = context_->getDevice();
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./data/Blur.spv", std::ios::binary);
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
    auto mapEntry = vk::SpecializationMapEntry{};
    mapEntry.constantID = 0;
    mapEntry.offset = 0;
    mapEntry.size = 4;
    auto specializationInfo = vk::SpecializationInfo{};
    specializationInfo.mapEntryCount = 1;
    specializationInfo.pMapEntries = &mapEntry;
    specializationInfo.dataSize = 4;
    specializationInfo.pData = nullptr;
    auto createInfo = vk::ComputePipelineCreateInfo{};
    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    createInfo.stage.module = *module;
    createInfo.stage.pName = "main";
    createInfo.stage.pSpecializationInfo = &specializationInfo;
    createInfo.layout = blurPipelineLayout_;
    createInfo.basePipelineIndex = -1;
    auto pipelines = absl::flat_hash_map<int, vk::Pipeline>{};
    for (auto kernelSize : std::vector<std::int32_t>{
             3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25}) {
      specializationInfo.pData = &kernelSize;
      pipelines.emplace(
          kernelSize, device.createComputePipeline({}, createInfo).value);
    }
    return pipelines;
  }

  absl::flat_hash_map<int, vk::Pipeline>
  SceneView::Flyweight::createBlurBlendPipelines() const {
    auto device = context_->getDevice();
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./data/BlurBlend.spv", std::ios::binary);
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
    auto mapEntry = vk::SpecializationMapEntry{};
    mapEntry.constantID = 0;
    mapEntry.offset = 0;
    mapEntry.size = 4;
    auto specializationInfo = vk::SpecializationInfo{};
    specializationInfo.mapEntryCount = 1;
    specializationInfo.pMapEntries = &mapEntry;
    specializationInfo.dataSize = 4;
    specializationInfo.pData = nullptr;
    auto createInfo = vk::ComputePipelineCreateInfo{};
    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    createInfo.stage.module = *module;
    createInfo.stage.pName = "main";
    createInfo.stage.pSpecializationInfo = &specializationInfo;
    createInfo.layout = blurBlendPipelineLayout_;
    createInfo.basePipelineIndex = -1;
    auto pipelines = absl::flat_hash_map<int, vk::Pipeline>{};
    for (auto kernelSize : std::vector<std::int32_t>{
             3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25}) {
      specializationInfo.pData = &kernelSize;
      pipelines.emplace(
          kernelSize, device.createComputePipeline({}, createInfo).value);
    }
    return pipelines;
  }

  vk::Pipeline SceneView::Flyweight::createBloomPipeline() const {
    auto device = context_->getDevice();
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./data/Bloom.spv", std::ios::binary);
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
    createInfo.layout = bloomPipelineLayout_;
    createInfo.basePipelineIndex = -1;
    return device.createComputePipeline({}, createInfo).value;
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

  vk::Sampler SceneView::Flyweight::createGeneralSampler() const {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  SceneView::Flyweight::~Flyweight() {
    auto device = context_->getDevice();
    device.destroy(bloomPipeline_);
    for (auto [_, pipeline] : blurBlendPipelines_) {
      device.destroy(pipeline);
    }
    for (auto [_, pipeline] : blurPipelines_) {
      device.destroy(pipeline);
    }
    device.destroy(downsamplePipeline_);
    device.destroy(renderPipeline_);
    device.destroy(skyViewPipeline_);
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

  vk::DescriptorSetLayout
  SceneView::Flyweight::getDownsampleDescriptorSetLayout() const noexcept {
    return downsampleDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getBlurDescriptorSetLayout() const noexcept {
    return blurDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getBlurBlendDescriptorSetLayout() const noexcept {
    return blurBlendDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getBloomDescriptorSetLayout() const noexcept {
    return bloomDescriptorSetLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getSkyViewPipelineLayout() const noexcept {
    return skyViewPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getRenderPipelineLayout() const noexcept {
    return renderPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getDownsamplePipelineLayout() const noexcept {
    return downsamplePipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getBlurPipelineLayout() const noexcept {
    return blurPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getBlurBlendPipelineLayout() const noexcept {
    return blurBlendPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getBloomPipelineLayout() const noexcept {
    return bloomPipelineLayout_;
  }

  vk::Pipeline SceneView::Flyweight::getSkyViewPipeline() const noexcept {
    return skyViewPipeline_;
  }

  vk::Pipeline SceneView::Flyweight::getRenderPipeline() const noexcept {
    return renderPipeline_;
  }

  vk::Pipeline SceneView::Flyweight::getDownsamplePipeline() const noexcept {
    return downsamplePipeline_;
  }

  vk::Pipeline
  SceneView::Flyweight::getBlurPipeline(int kernelSize) const noexcept {
    return blurPipelines_.at(kernelSize);
  }

  vk::Pipeline
  SceneView::Flyweight::getBlurBlendPipeline(int kernelSize) const noexcept {
    return blurBlendPipelines_.at(kernelSize);
  }

  vk::Pipeline SceneView::Flyweight::getBloomPipeline() const noexcept {
    return bloomPipeline_;
  }

  vk::Sampler SceneView::Flyweight::getSkyViewSampler() const noexcept {
    return skyViewSampler_;
  }

  vk::Sampler SceneView::Flyweight::getGeneralSampler() const noexcept {
    return generalSampler_;
  }

  SceneView::Frame::Frame(
      GpuImage &&renderImage,
      GpuImage &&smallBloomImageArray,
      GpuImage &&mediumBloomImageArray,
      GpuImage &&largeBloomImageArray):
      renderImage{std::move(renderImage)},
      smallBloomImageArray{std::move(smallBloomImageArray)},
      mediumBloomImageArray{std::move(mediumBloomImageArray)},
      largeBloomImageArray{std::move(largeBloomImageArray)} {}

  SceneView::SceneView(
      gsl::not_null<Flyweight const *> flyweight,
      gsl::not_null<std::shared_ptr<Scene>> scene,
      Extent2u const &extent) noexcept:
      flyweight_{flyweight},
      scene_{std::move(scene)},
      extent_{extent},
      uniformBuffer_{createUniformBuffer()},
      skyViewImage_{createSkyViewImage()},
      frames_{createFrames()},
      descriptorPool_{createDescriptorPool()},
      transitionCommandPool_{createTransitionCommandPool()},
      transitionCommandBuffer_{createTransitionCommandBuffer()},
      transitionSemaphore_{
          flyweight_->getContext()->getDevice().createSemaphore({})},
      transitionFence_{flyweight_->getContext()->getDevice().createFence({})},
      viewMatrix_{Eigen::Matrix4f::Identity()},
      invViewMatrix_{Eigen::Matrix4f::Identity()},
      projectionMatrix_{Eigen::Matrix4f::Identity()},
      invProjectionMatrix_{Eigen::Matrix4f::Identity()},
      exposure_{1.0f},
      smallBloomKernel_{15},
      mediumBloomKernel_{15},
      largeBloomKernel_{15},
      smallBloomWeight_{0.0006f},
      mediumBloomWeight_{0.0005f},
      largeBloomWeight_{0.0004f},
      firstFrame_{true} {
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      initSkyViewImageView(i);
      initRenderImageView(i);
      initHalfRenderImageView(i);
      initFourthRenderImageView(i);
      initEighthRenderImageView(i);
      initSmallBloomImageViews(i);
      initMediumBloomImageViews(i);
      initLargeBloomImageViews(i);
      allocateDescriptorSets(i);
      initSkyViewDescriptorSet(i);
      initRenderDescriptorSet(i);
      initDownsampleDescriptorSets(i);
      initSmallBlurDescriptorSets(i);
      initMediumBlurDescriptorSets(i);
      initLargeBlurDescriptorSets(i);
      initBloomDescriptorSet(i);
      initCommandPool(i);
      initCommandBuffers(i);
      initSemaphores(i);
    }
    auto submit = vk::SubmitInfo{};
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &transitionCommandBuffer_;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &transitionSemaphore_;
    flyweight_->getContext()->getComputeQueue().submit(
        submit, transitionFence_);
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

  std::vector<SceneView::Frame> SceneView::createFrames() const {
    auto frames = std::vector<SceneView::Frame>{};
    frames.reserve(flyweight_->getFrameCount());
    for (auto i = std::size_t{}; i < frames.capacity(); ++i) {
      frames.emplace_back(
          createRenderImage(),
          createSmallBloomImageArray(),
          createMediumBloomImageArray(),
          createLargeBloomImageArray());
    }
    return frames;
  }

  GpuImage SceneView::createRenderImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width, extent_.height, 1};
    image.mipLevels = 4;
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

  GpuImage SceneView::createSmallBloomImageArray() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width / 2, extent_.height / 2, 1};
    image.mipLevels = 1;
    image.arrayLayers = 2;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage =
        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  GpuImage SceneView::createMediumBloomImageArray() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width / 4, extent_.height / 4, 1};
    image.mipLevels = 1;
    image.arrayLayers = 2;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage =
        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  GpuImage SceneView::createLargeBloomImageArray() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width / 8, extent_.height / 8, 1};
    image.mipLevels = 1;
    image.arrayLayers = 2;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage =
        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  vk::DescriptorPool SceneView::createDescriptorPool() const {
    auto frameCount32 = static_cast<std::uint32_t>(flyweight_->getFrameCount());
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
        // sky view
        {vk::DescriptorType::eUniformBuffer, 2 * frameCount32},
        {vk::DescriptorType::eCombinedImageSampler, 1 * frameCount32},
        {vk::DescriptorType::eStorageImage, 1 * frameCount32},
        // render
        {vk::DescriptorType::eUniformBuffer, 2 * frameCount32},
        {vk::DescriptorType::eCombinedImageSampler, 2 * frameCount32},
        {vk::DescriptorType::eStorageImage, 1 * frameCount32},
        // downsample
        {vk::DescriptorType::eCombinedImageSampler, 3 * frameCount32},
        {vk::DescriptorType::eStorageImage, 3 * frameCount32},
        // blur
        {vk::DescriptorType::eCombinedImageSampler, 4 * frameCount32},
        {vk::DescriptorType::eStorageImage, 4 * frameCount32},
        // blurBlend
        {vk::DescriptorType::eCombinedImageSampler, 4 * frameCount32},
        {vk::DescriptorType::eStorageImage, 2 * frameCount32},
        // bloom
        {vk::DescriptorType::eCombinedImageSampler, 1 * frameCount32},
        {vk::DescriptorType::eStorageImage, 1 * frameCount32}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = 12 * frameCount32;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    return flyweight_->getContext()->getDevice().createDescriptorPool(
        createInfo);
  }

  vk::CommandPool SceneView::createTransitionCommandPool() const {
    auto &context = *flyweight_->getContext();
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.queueFamilyIndex = context.getComputeFamily();
    return context.getDevice().createCommandPool(createInfo);
  }

  vk::CommandBuffer SceneView::createTransitionCommandBuffer() {
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandPool = transitionCommandPool_;
    allocateInfo.commandBufferCount = 1;
    auto commandBuffer = vk::CommandBuffer{};
    flyweight_->getContext()->getDevice().allocateCommandBuffers(
        &allocateInfo, &commandBuffer);
    commandBuffer.begin(vk::CommandBufferBeginInfo{});
    auto barrier = vk::ImageMemoryBarrier{};
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = {};
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = skyViewImage_.get();
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = skyViewImage_.getMipLevels();
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = skyViewImage_.getArrayLayers();
    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        {},
        {},
        {},
        barrier);
    commandBuffer.end();
    return commandBuffer;
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

  void SceneView::initHalfRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 1;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].halfRenderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initFourthRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 2;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].fourthRenderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initEighthRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 3;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].eighthRenderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initSmallBloomImageViews(std::size_t i) {
    auto &frame = frames_[i];
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.smallBloomImageArray.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.smallBloomImageArray.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    for (auto j = 0; j < 2; ++j) {
      createInfo.subresourceRange.baseArrayLayer = j;
      frame.smallBloomImageViews[j] =
          flyweight_->getContext()->getDevice().createImageView(createInfo);
    }
  }

  void SceneView::initMediumBloomImageViews(std::size_t i) {
    auto &frame = frames_[i];
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.mediumBloomImageArray.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.mediumBloomImageArray.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    for (auto j = 0; j < 2; ++j) {
      createInfo.subresourceRange.baseArrayLayer = j;
      frame.mediumBloomImageViews[j] =
          flyweight_->getContext()->getDevice().createImageView(createInfo);
    }
  }

  void SceneView::initLargeBloomImageViews(std::size_t i) {
    auto &frame = frames_[i];
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.largeBloomImageArray.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.largeBloomImageArray.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    for (auto j = 0; j < 2; ++j) {
      createInfo.subresourceRange.baseArrayLayer = j;
      frame.largeBloomImageViews[j] =
          flyweight_->getContext()->getDevice().createImageView(createInfo);
    }
  }

  void SceneView::allocateDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    auto skyViewSetLayout = flyweight_->getSkyViewDescriptorSetLayout();
    auto renderSetLayout = flyweight_->getRenderDescriptorSetLayout();
    auto downsampleSetLayout = flyweight_->getDownsampleDescriptorSetLayout();
    auto blurSetLayout = flyweight_->getBlurDescriptorSetLayout();
    auto blurBlendSetLayout = flyweight_->getBlurBlendDescriptorSetLayout();
    auto bloomSetLayout = flyweight_->getBloomDescriptorSetLayout();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &skyViewSetLayout;
    device.allocateDescriptorSets(&allocateInfo, &frame.skyViewDescriptorSet);
    allocateInfo.pSetLayouts = &renderSetLayout;
    device.allocateDescriptorSets(&allocateInfo, &frame.renderDescriptorSet);
    allocateInfo.pSetLayouts = &downsampleSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.downsampleDescriptorSets[0]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.downsampleDescriptorSets[1]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.downsampleDescriptorSets[2]);
    allocateInfo.pSetLayouts = &blurSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.smallBlurDescriptorSets[0]);
    allocateInfo.pSetLayouts = &blurBlendSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.smallBlurDescriptorSets[1]);
    allocateInfo.pSetLayouts = &blurSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.mediumBlurDescriptorSets[0]);
    allocateInfo.pSetLayouts = &blurBlendSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.mediumBlurDescriptorSets[1]);
    allocateInfo.pSetLayouts = &blurSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.largeBlurDescriptorSets[0]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.largeBlurDescriptorSets[1]);
    allocateInfo.pSetLayouts = &bloomSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frames_[i].bloomDescriptorSet);
  }

  void SceneView::initSkyViewDescriptorSet(std::size_t i) {
    auto &frame = frames_[i];
    auto sceneViewBufferInfo = vk::DescriptorBufferInfo{};
    sceneViewBufferInfo.buffer = uniformBuffer_.get();
    sceneViewBufferInfo.offset = UNIFORM_BUFFER_STRIDE * i;
    sceneViewBufferInfo.range = UNIFORM_BUFFER_SIZE;
    auto skyViewImageInfo = vk::DescriptorImageInfo{};
    skyViewImageInfo.imageView = frame.skyViewImageView;
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
    auto &frame = frames_[i];
    auto sceneViewBufferInfo = vk::DescriptorBufferInfo{};
    sceneViewBufferInfo.buffer = uniformBuffer_.get();
    sceneViewBufferInfo.offset = UNIFORM_BUFFER_STRIDE * i;
    sceneViewBufferInfo.range = UNIFORM_BUFFER_SIZE;
    auto skyViewTextureInfo = vk::DescriptorImageInfo{};
    skyViewTextureInfo.sampler = flyweight_->getSkyViewSampler();
    skyViewTextureInfo.imageView = frame.skyViewImageView;
    skyViewTextureInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto renderImageInfo = vk::DescriptorImageInfo{};
    renderImageInfo.imageView = frame.renderImageView;
    renderImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto writes = std::array<vk::WriteDescriptorSet, 3>{};
    writes[0].dstSet = frame.renderDescriptorSet;
    writes[0].dstBinding = 1;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &sceneViewBufferInfo;
    writes[1].dstSet = frame.renderDescriptorSet;
    writes[1].dstBinding = 3;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writes[1].pImageInfo = &skyViewTextureInfo;
    writes[2].dstSet = frame.renderDescriptorSet;
    writes[2].dstBinding = 4;
    writes[2].dstArrayElement = 0;
    writes[2].descriptorCount = 1;
    writes[2].descriptorType = vk::DescriptorType::eStorageImage;
    writes[2].pImageInfo = &renderImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(writes, {});
  }

  void SceneView::initDownsampleDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto highResImageInfo = vk::DescriptorImageInfo{};
    highResImageInfo.sampler = flyweight_->getGeneralSampler();
    highResImageInfo.imageView = frame.renderImageView;
    highResImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto highResImageWrite = vk::WriteDescriptorSet{};
    highResImageWrite.dstSet = frame.downsampleDescriptorSets[0];
    highResImageWrite.dstBinding = 0;
    highResImageWrite.dstArrayElement = 0;
    highResImageWrite.descriptorCount = 1;
    highResImageWrite.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    highResImageWrite.pImageInfo = &highResImageInfo;
    auto lowResImageInfo = vk::DescriptorImageInfo{};
    lowResImageInfo.imageView = frame.halfRenderImageView;
    lowResImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto lowResImageWrite = vk::WriteDescriptorSet{};
    lowResImageWrite.dstSet = frame.downsampleDescriptorSets[0];
    lowResImageWrite.dstBinding = 1;
    lowResImageWrite.dstArrayElement = 0;
    lowResImageWrite.descriptorCount = 1;
    lowResImageWrite.descriptorType = vk::DescriptorType::eStorageImage;
    lowResImageWrite.pImageInfo = &lowResImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {highResImageWrite, lowResImageWrite}, {});
    highResImageInfo.imageView = frame.halfRenderImageView;
    highResImageWrite.dstSet = frame.downsampleDescriptorSets[1];
    lowResImageInfo.imageView = frame.fourthRenderImageView;
    lowResImageWrite.dstSet = frame.downsampleDescriptorSets[1];
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {highResImageWrite, lowResImageWrite}, {});
    highResImageInfo.imageView = frame.fourthRenderImageView;
    highResImageWrite.dstSet = frame.downsampleDescriptorSets[2];
    lowResImageInfo.imageView = frame.eighthRenderImageView;
    lowResImageWrite.dstSet = frame.downsampleDescriptorSets[2];
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {highResImageWrite, lowResImageWrite}, {});
  }

  void SceneView::initSmallBlurDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    {
      auto srcInfo = vk::DescriptorImageInfo{};
      srcInfo.sampler = flyweight_->getGeneralSampler();
      srcInfo.imageView = frame.halfRenderImageView;
      srcInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto srcWrite = vk::WriteDescriptorSet{};
      srcWrite.dstSet = frame.smallBlurDescriptorSets[0];
      srcWrite.dstBinding = 0;
      srcWrite.dstArrayElement = 0;
      srcWrite.descriptorCount = 1;
      srcWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      srcWrite.pImageInfo = &srcInfo;
      auto dstInfo = vk::DescriptorImageInfo{};
      dstInfo.imageView = frame.smallBloomImageViews[0];
      dstInfo.imageLayout = vk::ImageLayout::eGeneral;
      auto dstWrite = vk::WriteDescriptorSet{};
      dstWrite.dstSet = frame.smallBlurDescriptorSets[0];
      dstWrite.dstBinding = 1;
      dstWrite.dstArrayElement = 0;
      dstWrite.descriptorCount = 1;
      dstWrite.descriptorType = vk::DescriptorType::eStorageImage;
      dstWrite.pImageInfo = &dstInfo;
      device.updateDescriptorSets({srcWrite, dstWrite}, 0);
    }
    {
      auto blurSrcInfo = vk::DescriptorImageInfo{};
      blurSrcInfo.sampler = flyweight_->getGeneralSampler();
      blurSrcInfo.imageView = frame.smallBloomImageViews[0];
      blurSrcInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto blurSrcWrite = vk::WriteDescriptorSet{};
      blurSrcWrite.dstSet = frame.smallBlurDescriptorSets[1];
      blurSrcWrite.dstBinding = 0;
      blurSrcWrite.dstArrayElement = 0;
      blurSrcWrite.descriptorCount = 1;
      blurSrcWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      blurSrcWrite.pImageInfo = &blurSrcInfo;
      auto blendSrcInfo = vk::DescriptorImageInfo{};
      blendSrcInfo.sampler = flyweight_->getGeneralSampler();
      blendSrcInfo.imageView = frame.mediumBloomImageViews[1];
      blendSrcInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto blendSrcWrite = vk::WriteDescriptorSet{};
      blendSrcWrite.dstSet = frame.smallBlurDescriptorSets[1];
      blendSrcWrite.dstBinding = 1;
      blendSrcWrite.dstArrayElement = 0;
      blendSrcWrite.descriptorCount = 1;
      blendSrcWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      blendSrcWrite.pImageInfo = &blendSrcInfo;
      auto dstInfo = vk::DescriptorImageInfo{};
      dstInfo.imageView = frame.smallBloomImageViews[1];
      dstInfo.imageLayout = vk::ImageLayout::eGeneral;
      auto dstWrite = vk::WriteDescriptorSet{};
      dstWrite.dstSet = frame.smallBlurDescriptorSets[1];
      dstWrite.dstBinding = 2;
      dstWrite.dstArrayElement = 0;
      dstWrite.descriptorCount = 1;
      dstWrite.descriptorType = vk::DescriptorType::eStorageImage;
      dstWrite.pImageInfo = &dstInfo;
      device.updateDescriptorSets({blurSrcWrite, blendSrcWrite, dstWrite}, 0);
    }
  }

  void SceneView::initMediumBlurDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    {
      auto srcInfo = vk::DescriptorImageInfo{};
      srcInfo.sampler = flyweight_->getGeneralSampler();
      srcInfo.imageView = frame.fourthRenderImageView;
      srcInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto srcWrite = vk::WriteDescriptorSet{};
      srcWrite.dstSet = frame.mediumBlurDescriptorSets[0];
      srcWrite.dstBinding = 0;
      srcWrite.dstArrayElement = 0;
      srcWrite.descriptorCount = 1;
      srcWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      srcWrite.pImageInfo = &srcInfo;
      auto dstInfo = vk::DescriptorImageInfo{};
      dstInfo.imageView = frame.mediumBloomImageViews[0];
      dstInfo.imageLayout = vk::ImageLayout::eGeneral;
      auto dstWrite = vk::WriteDescriptorSet{};
      dstWrite.dstSet = frame.mediumBlurDescriptorSets[0];
      dstWrite.dstBinding = 1;
      dstWrite.dstArrayElement = 0;
      dstWrite.descriptorCount = 1;
      dstWrite.descriptorType = vk::DescriptorType::eStorageImage;
      dstWrite.pImageInfo = &dstInfo;
      device.updateDescriptorSets({srcWrite, dstWrite}, 0);
    }
    {
      auto blurSrcInfo = vk::DescriptorImageInfo{};
      blurSrcInfo.sampler = flyweight_->getGeneralSampler();
      blurSrcInfo.imageView = frame.mediumBloomImageViews[0];
      blurSrcInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto blurSrcWrite = vk::WriteDescriptorSet{};
      blurSrcWrite.dstSet = frame.mediumBlurDescriptorSets[1];
      blurSrcWrite.dstBinding = 0;
      blurSrcWrite.dstArrayElement = 0;
      blurSrcWrite.descriptorCount = 1;
      blurSrcWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      blurSrcWrite.pImageInfo = &blurSrcInfo;
      auto blendSrcInfo = vk::DescriptorImageInfo{};
      blendSrcInfo.sampler = flyweight_->getGeneralSampler();
      blendSrcInfo.imageView = frame.largeBloomImageViews[1];
      blendSrcInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto blendSrcWrite = vk::WriteDescriptorSet{};
      blendSrcWrite.dstSet = frame.mediumBlurDescriptorSets[1];
      blendSrcWrite.dstBinding = 1;
      blendSrcWrite.dstArrayElement = 0;
      blendSrcWrite.descriptorCount = 1;
      blendSrcWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      blendSrcWrite.pImageInfo = &blendSrcInfo;
      auto dstInfo = vk::DescriptorImageInfo{};
      dstInfo.imageView = frame.mediumBloomImageViews[1];
      dstInfo.imageLayout = vk::ImageLayout::eGeneral;
      auto dstWrite = vk::WriteDescriptorSet{};
      dstWrite.dstSet = frame.mediumBlurDescriptorSets[1];
      dstWrite.dstBinding = 2;
      dstWrite.dstArrayElement = 0;
      dstWrite.descriptorCount = 1;
      dstWrite.descriptorType = vk::DescriptorType::eStorageImage;
      dstWrite.pImageInfo = &dstInfo;
      device.updateDescriptorSets({blurSrcWrite, blendSrcWrite, dstWrite}, 0);
    }
  }

  void SceneView::initLargeBlurDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    {
      auto srcInfo = vk::DescriptorImageInfo{};
      srcInfo.sampler = flyweight_->getGeneralSampler();
      srcInfo.imageView = frame.eighthRenderImageView;
      srcInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto srcWrite = vk::WriteDescriptorSet{};
      srcWrite.dstSet = frame.largeBlurDescriptorSets[0];
      srcWrite.dstBinding = 0;
      srcWrite.dstArrayElement = 0;
      srcWrite.descriptorCount = 1;
      srcWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      srcWrite.pImageInfo = &srcInfo;
      auto dstInfo = vk::DescriptorImageInfo{};
      dstInfo.imageView = frame.largeBloomImageViews[0];
      dstInfo.imageLayout = vk::ImageLayout::eGeneral;
      auto dstWrite = vk::WriteDescriptorSet{};
      dstWrite.dstSet = frame.largeBlurDescriptorSets[0];
      dstWrite.dstBinding = 1;
      dstWrite.dstArrayElement = 0;
      dstWrite.descriptorCount = 1;
      dstWrite.descriptorType = vk::DescriptorType::eStorageImage;
      dstWrite.pImageInfo = &dstInfo;
      device.updateDescriptorSets({srcWrite, dstWrite}, 0);
      srcInfo.imageView = frame.largeBloomImageViews[0];
      srcWrite.dstSet = frame.largeBlurDescriptorSets[1];
      dstInfo.imageView = frame.largeBloomImageViews[1];
      dstWrite.dstSet = frame.largeBlurDescriptorSets[1];
      device.updateDescriptorSets({srcWrite, dstWrite}, 0);
    }
  }

  void SceneView::initBloomDescriptorSet(std::size_t i) {
    auto renderImageInfo = vk::DescriptorImageInfo{};
    renderImageInfo.imageView = frames_[i].renderImageView;
    renderImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto renderImageWrite = vk::WriteDescriptorSet{};
    renderImageWrite.dstSet = frames_[i].bloomDescriptorSet;
    renderImageWrite.dstBinding = 0;
    renderImageWrite.dstArrayElement = 0;
    renderImageWrite.descriptorCount = 1;
    renderImageWrite.descriptorType = vk::DescriptorType::eStorageImage;
    renderImageWrite.pImageInfo = &renderImageInfo;
    auto bloomImageInfo = vk::DescriptorImageInfo{};
    bloomImageInfo.sampler = flyweight_->getGeneralSampler();
    bloomImageInfo.imageView = frames_[i].smallBloomImageViews[1];
    bloomImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto bloomImageWrite = vk::WriteDescriptorSet{};
    bloomImageWrite.dstSet = frames_[i].bloomDescriptorSet;
    bloomImageWrite.dstBinding = 1;
    bloomImageWrite.dstArrayElement = 0;
    bloomImageWrite.descriptorCount = 1;
    bloomImageWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bloomImageWrite.pImageInfo = &bloomImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {renderImageWrite, bloomImageWrite}, {});
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
    device.allocateCommandBuffers(&allocateInfo, &frames_[i].commandBuffer);
  }

  void SceneView::initSemaphores(std::size_t i) {
    auto device = flyweight_->getContext()->getDevice();
    frames_[i].semaphore = device.createSemaphore({});
  }

  SceneView::~SceneView() {
    auto device = flyweight_->getContext()->getDevice();
    for (auto &frame : frames_) {
      device.destroy(frame.semaphore);
      device.destroy(frame.commandPool);
      for (auto imageView : frame.largeBloomImageViews) {
        device.destroy(imageView);
      }
      for (auto imageView : frame.mediumBloomImageViews) {
        device.destroy(imageView);
      }
      for (auto imageView : frame.smallBloomImageViews) {
        device.destroy(imageView);
      }
      device.destroy(frame.eighthRenderImageView);
      device.destroy(frame.fourthRenderImageView);
      device.destroy(frame.halfRenderImageView);
      device.destroy(frame.renderImageView);
      device.destroy(frame.skyViewImageView);
    }
    device.waitForFences(
        transitionFence_, false, std::numeric_limits<std::uint64_t>::max());
    device.destroy(transitionFence_);
    device.destroy(transitionSemaphore_);
    device.destroy(transitionCommandPool_);
    device.destroy(descriptorPool_);
  }

  void SceneView::render(std::size_t i) {
    auto &context = *flyweight_->getContext();
    auto queue = context.getComputeQueue();
    auto device = context.getDevice();
    auto &frame = frames_[i];
    updateUniformBuffer(i);
    if (frame.renderImage.getExtent() !=
        Extent3u{extent_.width, extent_.height, 1}) {
      updateRenderImages(i);
    }
    if (frame.scene != scene_) {
      updateSkyViewDescriptorSet(i);
      updateRenderDescriptorSet(i);
      frame.scene = scene_;
    }
    device.resetCommandPool(frame.commandPool);
    submitCommands(i);
    firstFrame_ = false;
  }

  void SceneView::updateRenderImages(std::size_t i) {
    auto device = flyweight_->getContext()->getDevice();
    auto &frame = frames_[i];
    for (auto imageView : frame.largeBloomImageViews) {
      device.destroy(imageView);
    }
    for (auto imageView : frame.mediumBloomImageViews) {
      device.destroy(imageView);
    }
    for (auto imageView : frame.smallBloomImageViews) {
      device.destroy(imageView);
    }
    device.destroy(frame.eighthRenderImageView);
    device.destroy(frame.fourthRenderImageView);
    device.destroy(frame.halfRenderImageView);
    device.destroy(frame.renderImageView);
    frame.renderImage = createRenderImage();
    frame.smallBloomImageArray = createSmallBloomImageArray();
    frame.mediumBloomImageArray = createMediumBloomImageArray();
    frame.largeBloomImageArray = createLargeBloomImageArray();
    initRenderImageView(i);
    initHalfRenderImageView(i);
    initFourthRenderImageView(i);
    initEighthRenderImageView(i);
    initSmallBloomImageViews(i);
    initMediumBloomImageViews(i);
    initLargeBloomImageViews(i);
    initRenderDescriptorSet(i);
    initDownsampleDescriptorSets(i);
    initSmallBlurDescriptorSets(i);
    initMediumBlurDescriptorSets(i);
    initLargeBlurDescriptorSets(i);
    initBloomDescriptorSet(i);
  }

  void SceneView::updateUniformBuffer(std::size_t frameIndex) {
    using Eigen::Matrix4f;
    using Eigen::Vector3f;
    using Eigen::Vector4f;
    Vector3f position = invViewMatrix_.col(3).head<3>();
    Vector3f zenith = position - scene_->getPlanet()->getPosition();
    auto radius = zenith.norm();
    auto altitude = radius - scene_->getPlanet()->getGroundRadius();
    zenith /= radius;
    Vector3f tangent =
        std::abs(zenith.x()) > std::abs(zenith.y())
            ? Vector3f{-zenith.z(), 0.0f, zenith.x()} /
                  std::sqrt(zenith.x() * zenith.x() + zenith.z() * zenith.z())
            : Vector3f{0.0f, zenith.z(), -zenith.y()} /
                  std::sqrt(zenith.y() * zenith.y() + zenith.z() * zenith.z());
    Vector3f bitangent = zenith.cross(tangent);
    Matrix4f skyViewMatrix = Matrix4f::Identity();
    skyViewMatrix.col(0).head<3>() = tangent;
    skyViewMatrix.col(1).head<3>() = bitangent;
    skyViewMatrix.col(2).head<3>() = zenith;
    skyViewMatrix.col(3).head<3>() = position;
    skyViewMatrix = skyViewMatrix.inverse().eval();
    Matrix4f viewDirectionMatrix = Eigen::Matrix4f::Identity();
    viewDirectionMatrix.topLeftCorner<3, 3>() =
        skyViewMatrix.topLeftCorner<3, 3>() *
        invViewMatrix_.topLeftCorner<3, 3>();
    viewDirectionMatrix *= invProjectionMatrix_;
    auto viewDirections = std::array{
        Vector4f{-1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{-1.0f, -1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, -1.0f, 1.0f, 1.0f}};
    for (auto &viewDirection : viewDirections) {
      viewDirection = viewDirectionMatrix * viewDirection;
      viewDirection *= 1.0f / viewDirection.w();
      viewDirection.head<3>().normalize();
    }
    Vector3f sunDirection = skyViewMatrix.topLeftCorner<3, 3>() *
                            scene_->getSunLight()->getDirection();
    auto groundLat =
        -std::acos(scene_->getPlanet()->getGroundRadius() / radius);
    auto atmosphereLat =
        scene_->getPlanet()->getAtmosphereRadius() < radius
            ? -std::acos(scene_->getPlanet()->getAtmosphereRadius() / radius)
            : 0.5f * PI<float>;
    auto offset = UNIFORM_BUFFER_STRIDE * frameIndex;
    auto data = uniformBuffer_.getMappedData() + offset;
    std::memcpy(data + 0, &viewDirections[0], 12);
    std::memcpy(data + 16, &viewDirections[1], 12);
    std::memcpy(data + 32, &viewDirections[2], 12);
    std::memcpy(data + 48, &viewDirections[3], 12);
    std::memcpy(data + 64, &sunDirection, 12);
    std::memcpy(data + 76, &altitude, 4);
    std::memcpy(data + 80, &groundLat, 4);
    std::memcpy(data + 84, &atmosphereLat, 4);
    std::memcpy(data + 88, &exposure_, 4);
    uniformBuffer_.flush(offset, UNIFORM_BUFFER_SIZE);
  }

  void SceneView::updateSkyViewDescriptorSet(std::size_t i) {
    auto sceneBufferInfo = vk::DescriptorBufferInfo{};
    sceneBufferInfo.buffer = scene_->getUniformBuffer().get();
    sceneBufferInfo.offset = Scene::UNIFORM_BUFFER_STRIDE * i;
    sceneBufferInfo.range = Scene::UNIFORM_BUFFER_SIZE;
    auto transmittanceImageInfo = vk::DescriptorImageInfo{};
    transmittanceImageInfo.sampler =
        scene_->getFlyweight()->getTransmittanceSampler();
    transmittanceImageInfo.imageView = scene_->getTransmittanceImageView(i);
    transmittanceImageInfo.imageLayout =
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
    writes[1].pImageInfo = &transmittanceImageInfo;
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
    auto transmittanceImageInfo = vk::DescriptorImageInfo{};
    transmittanceImageInfo.sampler =
        scene_->getFlyweight()->getTransmittanceSampler();
    transmittanceImageInfo.imageView = scene_->getTransmittanceImageView(i);
    transmittanceImageInfo.imageLayout =
        vk::ImageLayout::eShaderReadOnlyOptimal;
    auto transmittanceLutWrite = vk::WriteDescriptorSet{};
    transmittanceLutWrite.dstSet = frames_[i].renderDescriptorSet;
    transmittanceLutWrite.dstBinding = 2;
    transmittanceLutWrite.dstArrayElement = 0;
    transmittanceLutWrite.descriptorCount = 1;
    transmittanceLutWrite.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    transmittanceLutWrite.pImageInfo = &transmittanceImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {sceneBufferWrite, transmittanceLutWrite}, {});
  }

  void SceneView::submitCommands(std::size_t i) {
    auto &frame = frames_[i];
    frame.commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    computeSkyViewImage(i);
    computeRenderImage(i);
    computeRenderImageMips(i);
    computeBloomImageArray(i);
    applyBloom(i);
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask =
          vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
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
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eBottomOfPipe,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.end();
    auto waitSemaphores = std::vector<vk::Semaphore>{scene_->getSemaphore(i)};
    auto waitStages = std::vector<vk::PipelineStageFlags>{
        vk::PipelineStageFlagBits::eComputeShader};
    if (firstFrame_) {
      waitSemaphores.emplace_back(transitionSemaphore_);
      waitStages.emplace_back(vk::PipelineStageFlagBits::eTopOfPipe);
    }
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.waitSemaphoreCount =
        static_cast<std::uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frame.semaphore;
    flyweight_->getContext()->getComputeQueue().submit(submitInfo);
  }

  void SceneView::computeSkyViewImage(std::size_t i) {
    auto &frame = frames_[i];
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getSkyViewPipeline());
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getSkyViewPipelineLayout(),
        0,
        frame.skyViewDescriptorSet,
        {});
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = skyViewImage_.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (skyViewImage_.getExtent().width + 7) / 8,
        (skyViewImage_.getExtent().height + 7) / 8,
        1);
  }

  void SceneView::computeRenderImage(std::size_t i) {
    auto &frame = frames_[i];
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getRenderPipeline());
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getRenderPipelineLayout(),
        0,
        frame.renderDescriptorSet,
        {});
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
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
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
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
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.renderImage.getExtent().width + 7) / 8,
        (frame.renderImage.getExtent().height + 7) / 8,
        1);
  }

  void SceneView::computeRenderImageMips(std::size_t i) {
    auto &frame = frames_[i];
    auto barrier = vk::ImageMemoryBarrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = frame.renderImage.get();
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getDownsamplePipeline());
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getDownsamplePipelineLayout(),
        0,
        frame.downsampleDescriptorSets[0],
        {});
    barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    barrier.oldLayout = vk::ImageLayout::eGeneral;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.subresourceRange.baseMipLevel = 0;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        barrier);
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eGeneral;
    barrier.subresourceRange.baseMipLevel = 1;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        barrier);
    frame.commandBuffer.dispatch(
        (frame.renderImage.getExtent().width / 2 + 7) / 8,
        (frame.renderImage.getExtent().height / 2 + 7) / 8,
        1);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getDownsamplePipelineLayout(),
        0,
        frame.downsampleDescriptorSets[1],
        {});
    barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    barrier.oldLayout = vk::ImageLayout::eGeneral;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.subresourceRange.baseMipLevel = 1;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        barrier);
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eGeneral;
    barrier.subresourceRange.baseMipLevel = 2;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        barrier);
    frame.commandBuffer.dispatch(
        (frame.renderImage.getExtent().width / 4 + 7) / 8,
        (frame.renderImage.getExtent().height / 4 + 7) / 8,
        1);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getDownsamplePipelineLayout(),
        0,
        frame.downsampleDescriptorSets[2],
        {});
    barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    barrier.oldLayout = vk::ImageLayout::eGeneral;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.subresourceRange.baseMipLevel = 2;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        barrier);
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eGeneral;
    barrier.subresourceRange.baseMipLevel = 3;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        barrier);
    frame.commandBuffer.dispatch(
        (frame.renderImage.getExtent().width / 8 + 7) / 8,
        (frame.renderImage.getExtent().height / 8 + 7) / 8,
        1);
  }

  void SceneView::computeBloomImageArray(std::size_t i) {
    auto &frame = frames_[i];
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(largeBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.largeBlurDescriptorSets[0],
        {});
    {
      auto pushConstants = std::array{0};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 3;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.largeBloomImageArray.getExtent().width + 7) / 8,
        (frame.largeBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(mediumBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.mediumBlurDescriptorSets[0],
        {});
    {
      auto pushConstants = std::array{0};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.mediumBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.mediumBloomImageArray.getExtent().width + 7) / 8,
        (frame.mediumBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(smallBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.smallBlurDescriptorSets[0],
        {});
    {
      auto pushConstants = std::array{0};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.smallBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.smallBloomImageArray.getExtent().width + 7) / 8,
        (frame.smallBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(largeBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.largeBlurDescriptorSets[1],
        {});
    {
      auto pushConstants = std::array{1};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.largeBloomImageArray.getExtent().width + 7) / 8,
        (frame.largeBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipeline(mediumBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipelineLayout(),
        0,
        frame.mediumBlurDescriptorSets[1],
        {});
    {
      struct {
        int blurDirection;
        float blurWeight;
        float blendWeight;
      } pushConstants;
      pushConstants.blurDirection = 1;
      pushConstants.blurWeight = mediumBloomWeight_;
      pushConstants.blendWeight = largeBloomWeight_;
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurBlendPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          12,
          &pushConstants);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.mediumBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.mediumBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.mediumBloomImageArray.getExtent().width + 7) / 8,
        (frame.mediumBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipeline(smallBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipelineLayout(),
        0,
        frame.smallBlurDescriptorSets[1],
        {});
    {
      struct {
        int blurDirection;
        float blurWeight;
        float blendWeight;
      } pushConstants;
      pushConstants.blurDirection = 1;
      pushConstants.blurWeight = smallBloomWeight_;
      pushConstants.blendWeight = 1.0f;
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurBlendPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          12,
          &pushConstants);
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        barrier.oldLayout = vk::ImageLayout::eGeneral;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = frame.smallBloomImageArray.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        frame.commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eComputeShader,
            vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        barrier.oldLayout = vk::ImageLayout::eGeneral;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = frame.mediumBloomImageArray.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 1;
        barrier.subresourceRange.layerCount = 1;
        frame.commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eComputeShader,
            vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.oldLayout = vk::ImageLayout::eUndefined;
        barrier.newLayout = vk::ImageLayout::eGeneral;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = frame.smallBloomImageArray.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 1;
        barrier.subresourceRange.layerCount = 1;
        frame.commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      frame.commandBuffer.dispatch(
          (frame.smallBloomImageArray.getExtent().width + 7) / 8,
          (frame.smallBloomImageArray.getExtent().height + 7) / 8,
          1);
    }
  }

  void SceneView::applyBloom(std::size_t i) {
    auto &frame = frames_[i];
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getBloomPipeline());
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBloomPipelineLayout(),
        0,
        frame.bloomDescriptorSet,
        {});
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.smallBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.dstAccessMask =
          vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.renderImage.getExtent().width + 7) / 8,
        (frame.renderImage.getExtent().height + 7) / 8,
        1);
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

  vk::ImageView
  SceneView::getFullRenderImageView(std::size_t i) const noexcept {
    return frames_[i].renderImageView;
  }

  vk::Semaphore SceneView::getSemaphore(std::size_t i) const noexcept {
    return frames_[i].semaphore;
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

  float SceneView::getExposure() const noexcept {
    return exposure_;
  }

  void SceneView::setExposure(float exposure) noexcept {
    exposure_ = exposure;
  }
} // namespace imp