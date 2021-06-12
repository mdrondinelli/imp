#include "SkyViewLut.h"

#include <fstream>
#include <vector>

#include "AtmosphereBuffer.h"
#include "Scene.h"
#include "TransmittanceLut.h"

namespace imp {
  SkyViewLut::Flyweight::Flyweight(GpuContext *context):
      context_{context},
      computeDescriptorSetLayout_{createComputeDescriptorSetLayout()},
      renderDescriptorSetLayout_{createRenderDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()} {}

  GpuContext *SkyViewLut::Flyweight::getContext() const noexcept {
    return context_;
  }

  vk::DescriptorSetLayout
  SkyViewLut::Flyweight::getComputeDescriptorSetLayout() const noexcept {
    return computeDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SkyViewLut::Flyweight::getRenderDescriptorSetLayout() const noexcept {
    return renderDescriptorSetLayout_;
  }

  vk::PipelineLayout SkyViewLut::Flyweight::getPipelineLayout() const noexcept {
    return *pipelineLayout_;
  }

  vk::Pipeline SkyViewLut::Flyweight::getPipeline() const noexcept {
    return *pipeline_;
  }

  vk::Sampler SkyViewLut::Flyweight::getSampler() const noexcept {
    return sampler_;
  }

  vk::DescriptorSetLayout
  SkyViewLut::Flyweight::createComputeDescriptorSetLayout() {
    auto bufferBinding = GpuDescriptorSetLayoutBinding{};
    bufferBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    bufferBinding.descriptorCount = 1;
    bufferBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto transmittanceTextureBinding = GpuDescriptorSetLayoutBinding{};
    transmittanceTextureBinding.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    transmittanceTextureBinding.descriptorCount = 1;
    transmittanceTextureBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto skyViewImageBinding = GpuDescriptorSetLayoutBinding{};
    skyViewImageBinding.descriptorType = vk::DescriptorType::eStorageImage;
    skyViewImageBinding.descriptorCount = 1;
    skyViewImageBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto bindings = std::array{
        bufferBinding, transmittanceTextureBinding, skyViewImageBinding};
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
    createInfo.bindings = bindings.data();
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SkyViewLut::Flyweight::createRenderDescriptorSetLayout() {
    auto binding = GpuDescriptorSetLayoutBinding{};
    binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    binding.descriptorCount = 1;
    binding.stageFlags =
        vk::ShaderStageFlagBits::eCompute | vk::ShaderStageFlagBits::eFragment;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindingCount = 1;
    createInfo.bindings = &binding;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::UniquePipelineLayout SkyViewLut::Flyweight::createPipelineLayout() {
    auto pushConstantRange = vk::PushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 32;
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &computeDescriptorSetLayout_;
    createInfo.pushConstantRangeCount = 1;
    createInfo.pPushConstantRanges = &pushConstantRange;
    return context_->getDevice().createPipelineLayoutUnique(createInfo);
  }

  vk::UniquePipeline SkyViewLut::Flyweight::createPipeline() {
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./res/SkyView.spv", std::ios::binary);
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
    auto module =
        context_->getDevice().createShaderModuleUnique(moduleCreateInfo);
    auto createInfo = vk::ComputePipelineCreateInfo{};
    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    createInfo.stage.module = *module;
    createInfo.stage.pName = "main";
    createInfo.layout = *pipelineLayout_;
    createInfo.basePipelineIndex = -1;
    return context_->getDevice()
        .createComputePipelineUnique({}, createInfo)
        .value;
  }

  vk::Sampler SkyViewLut::Flyweight::createSampler() {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  SkyViewLut::SkyViewLut(
      Flyweight const *flyweight,
      TransmittanceLut const *transmittanceLut,
      Vector2u const &size):
      flyweight_{flyweight},
      buffer_{transmittanceLut->getBuffer()},
      transmittanceLut_{transmittanceLut},
      size_{size},
      image_{createImage()},
      imageView_{createImageView()},
      descriptorPool_{createDescriptorPool()},
      descriptorSets_{allocateDescriptorSets()} {
    updateDescriptorSets();
  }

  SkyViewLut::Flyweight const *SkyViewLut::getFlyweight() const noexcept {
    return flyweight_;
  }

  AtmosphereBuffer const *SkyViewLut::getBuffer() const noexcept {
    return buffer_;
  }

  TransmittanceLut const *SkyViewLut::getTransmittanceLut() const noexcept {
    return transmittanceLut_;
  }

  Vector2u const &SkyViewLut::getSize() const noexcept {
    return size_;
  }

  vk::Image SkyViewLut::getImage() const noexcept {
    return image_.get();
  }

  vk::ImageView SkyViewLut::getImageView() const noexcept {
    return imageView_.get();
  }

  vk::DescriptorSet SkyViewLut::getComputeDescriptorSet() const noexcept {
    return descriptorSets_[0];
  }

  vk::DescriptorSet SkyViewLut::getRenderDescriptorSet() const noexcept {
    return descriptorSets_[1];
  }

  void SkyViewLut::compute(
      vk::CommandBuffer cmd,
      DirectionalLight const &sun,
      Camera const &camera) {
    cmd.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getPipeline());
    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getPipelineLayout(),
        0,
        {getComputeDescriptorSet()},
        {});
    auto pushConstants = std::array<char, 32>{};
    std::memcpy(&pushConstants[0], &sun.getIrradiance(), 12);
    std::memcpy(&pushConstants[16], &sun.getDirection(), 12);
    std::memcpy(
        &pushConstants[28], &camera.getTransform().getTranslation()[1], 4);
    cmd.pushConstants(
        flyweight_->getPipelineLayout(),
        vk::ShaderStageFlagBits::eCompute,
        0,
        static_cast<std::uint32_t>(pushConstants.size()),
        pushConstants.data());
    auto barrier = vk::ImageMemoryBarrier{};
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eGeneral;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image_.get();
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        barrier);
    cmd.dispatch(size_[0] / 8u, size_[1] / 8u, 1u);
  }

  GpuImage SkyViewLut::createImage() {
    auto createInfo = GpuImageCreateInfo{};
    createInfo.image.imageType = vk::ImageType::e2D;
    createInfo.image.format = vk::Format::eR16G16B16A16Sfloat;
    createInfo.image.extent.width = size_[0];
    createInfo.image.extent.height = size_[1];
    createInfo.image.extent.depth = 1;
    createInfo.image.mipLevels = 1;
    createInfo.image.arrayLayers = 1;
    createInfo.image.samples = vk::SampleCountFlagBits::e1;
    createInfo.image.tiling = vk::ImageTiling::eOptimal;
    createInfo.image.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
    createInfo.image.sharingMode = vk::SharingMode::eExclusive;
    createInfo.image.initialLayout = vk::ImageLayout::eUndefined;
    createInfo.allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{*flyweight_->getContext(), createInfo};
  }

  vk::UniqueImageView SkyViewLut::createImageView() {
    auto viewInfo = vk::ImageViewCreateInfo{};
    viewInfo.image = image_.get();
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = vk::Format::eR16G16B16A16Sfloat;
    viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
    viewInfo.components.a = vk::ComponentSwizzle::eIdentity;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    return flyweight_->getContext()->getDevice().createImageViewUnique(
        viewInfo);
  }

  vk::UniqueDescriptorPool SkyViewLut::createDescriptorPool() {
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
        {vk::DescriptorType::eStorageImage, 1},
        {vk::DescriptorType::eCombinedImageSampler, 1}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = 2;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    return flyweight_->getContext()->getDevice().createDescriptorPoolUnique(
        createInfo);
  }

  std::vector<vk::DescriptorSet> SkyViewLut::allocateDescriptorSets() {
    auto setLayouts = std::array{
        flyweight_->getComputeDescriptorSetLayout(),
        flyweight_->getRenderDescriptorSetLayout()};
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = *descriptorPool_;
    allocateInfo.descriptorSetCount =
        static_cast<std::uint32_t>(setLayouts.size());
    allocateInfo.pSetLayouts = setLayouts.data();
    return flyweight_->getContext()->getDevice().allocateDescriptorSets(
        allocateInfo);
  }

  void SkyViewLut::updateDescriptorSets() {
    auto bufferInfo = vk::DescriptorBufferInfo{};
    bufferInfo.buffer = buffer_->get();
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;
    auto bufferWrite = vk::WriteDescriptorSet{};
    bufferWrite.dstSet = getComputeDescriptorSet();
    bufferWrite.dstBinding = 0;
    bufferWrite.dstArrayElement = 0;
    bufferWrite.descriptorCount = 1;
    bufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    bufferWrite.pBufferInfo = &bufferInfo;
    auto transmittanceInfo = vk::DescriptorImageInfo{};
    transmittanceInfo.sampler = transmittanceLut_->getFlyweight()->getSampler();
    transmittanceInfo.imageView = transmittanceLut_->getImageView();
    transmittanceInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto transmittanceWrite = vk::WriteDescriptorSet{};
    transmittanceWrite.dstSet = getComputeDescriptorSet();
    transmittanceWrite.dstBinding = 1;
    transmittanceWrite.dstArrayElement = 0;
    transmittanceWrite.descriptorCount = 1;
    transmittanceWrite.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    transmittanceWrite.pImageInfo = &transmittanceInfo;
    auto skyViewInfo = vk::DescriptorImageInfo{};
    skyViewInfo.imageView = *imageView_;
    skyViewInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto skyViewWrite = vk::WriteDescriptorSet{};
    skyViewWrite.dstSet = getComputeDescriptorSet();
    skyViewWrite.dstBinding = 2;
    skyViewWrite.dstArrayElement = 0;
    skyViewWrite.descriptorCount = 1;
    skyViewWrite.descriptorType = vk::DescriptorType::eStorageImage;
    skyViewWrite.pImageInfo = &skyViewInfo;
    auto renderInfo = vk::DescriptorImageInfo{};
    renderInfo.sampler = flyweight_->getSampler();
    renderInfo.imageView = *imageView_;
    renderInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto renderWrite = vk::WriteDescriptorSet{};
    renderWrite.dstSet = getRenderDescriptorSet();
    renderWrite.dstBinding = 0;
    renderWrite.dstArrayElement = 0;
    renderWrite.descriptorCount = 1;
    renderWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    renderWrite.pImageInfo = &renderInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {bufferWrite, transmittanceWrite, skyViewWrite, renderWrite}, {});
  }
} // namespace imp