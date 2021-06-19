#include "TransmittanceLut.h"

#include <fstream>
#include <vector>

#include "../system/GpuContext.h"
#include "AtmosphereBuffer.h"
#include "Scene.h"

namespace imp {
  TransmittanceLut::Flyweight::Flyweight(GpuContext *context):
      context_{context},
      computeDescriptorSetLayout_{createComputeDescriptorSetLayout()},
      renderDescriptorSetLayout_{createRenderDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()} {}

  GpuContext *TransmittanceLut::Flyweight::getContext() const noexcept {
    return context_;
  }

  vk::DescriptorSetLayout
  TransmittanceLut::Flyweight::getComputeDescriptorSetLayout() const noexcept {
    return computeDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  TransmittanceLut::Flyweight::getRenderDescriptorSetLayout() const noexcept {
    return renderDescriptorSetLayout_;
  }

  vk::PipelineLayout
  TransmittanceLut::Flyweight::getPipelineLayout() const noexcept {
    return *pipelineLayout_;
  }

  vk::Pipeline TransmittanceLut::Flyweight::getPipeline() const noexcept {
    return *pipeline_;
  }

  vk::Sampler TransmittanceLut::Flyweight::getSampler() const noexcept {
    return sampler_;
  }

  vk::DescriptorSetLayout
  TransmittanceLut::Flyweight::createComputeDescriptorSetLayout() {
    auto bufferBinding = GpuDescriptorSetLayoutBinding{};
    bufferBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    bufferBinding.descriptorCount = 1;
    bufferBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto imageBinding = GpuDescriptorSetLayoutBinding{};
    imageBinding.descriptorType = vk::DescriptorType::eStorageImage;
    imageBinding.descriptorCount = 1;
    imageBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto bindings = std::array{bufferBinding, imageBinding};
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
    createInfo.bindings = bindings.data();
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  TransmittanceLut::Flyweight::createRenderDescriptorSetLayout() {
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

  vk::UniquePipelineLayout TransmittanceLut::Flyweight::createPipelineLayout() {
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &computeDescriptorSetLayout_;
    return context_->getDevice().createPipelineLayoutUnique(createInfo);
  }

  vk::UniquePipeline TransmittanceLut::Flyweight::createPipeline() {
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
    pipelineCreateInfo.layout = *pipelineLayout_;
    pipelineCreateInfo.basePipelineIndex = -1;
    return context_->getDevice()
        .createComputePipelineUnique({}, pipelineCreateInfo)
        .value;
  }

  vk::Sampler TransmittanceLut::Flyweight::createSampler() {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  TransmittanceLut::TransmittanceLut(
      Flyweight const *flyweight,
      AtmosphereBuffer const *buffer,
      unsigned width,
      unsigned height):
      flyweight_{flyweight},
      buffer_{buffer},
      width_{width},
      height_{height},
      image_{createImage()},
      imageView_{createImageView()},
      descriptorPool_{createDescriptorPool()},
      descriptorSets_{allocateDescriptorSets()},
      planetRadius_{0.0f},
      atmosphereRadius_{0.0f},
      rayleighScattering_{0.0f},
      rayleighScaleHeight_{0.0f},
      mieExtinction_{0.0f},
      mieScaleHeight_{0.0f},
      ozoneAbsorption_{0.0f},
      ozoneLayerHeight_{0.0f},
      ozoneLayerThickness_{0.0f} {
    updateDescriptorSets();
  }

  TransmittanceLut::Flyweight const *
  TransmittanceLut::getFlyweight() const noexcept {
    return flyweight_;
  }

  AtmosphereBuffer const *TransmittanceLut::getBuffer() const noexcept {
    return buffer_;
  }

  unsigned TransmittanceLut::getWidth() const noexcept {
    return width_;
  }

  unsigned TransmittanceLut::getHeight() const noexcept {
    return height_;
  }

  vk::Image TransmittanceLut::getImage() const noexcept {
    return image_.get();
  }

  vk::ImageView TransmittanceLut::getImageView() const noexcept {
    return *imageView_;
  }

  vk::DescriptorSet TransmittanceLut::getComputeDescriptorSet() const noexcept {
    return descriptorSets_[0];
  }

  vk::DescriptorSet TransmittanceLut::getRenderDescriptorSet() const noexcept {
    return descriptorSets_[1];
  }

  bool TransmittanceLut::compute(
      vk::CommandBuffer cmd, Atmosphere const &atmosphere) {
    if (rayleighScattering_ != atmosphere.getRayleighScattering() ||
        rayleighScaleHeight_ != atmosphere.getRayleighScaleHeight() ||
        mieExtinction_ !=
            atmosphere.getMieScattering() + atmosphere.getMieAbsorption() ||
        mieScaleHeight_ != atmosphere.getMieScaleHeight() ||
        ozoneAbsorption_ != atmosphere.getOzoneAbsorption() ||
        ozoneLayerHeight_ != atmosphere.getOzoneLayerHeight() ||
        ozoneLayerThickness_ != atmosphere.getOzoneLayerThickness() ||
        planetRadius_ != atmosphere.getPlanetRadius() ||
        atmosphereRadius_ != atmosphere.getAtmosphereRadius()) {
      planetRadius_ = atmosphere.getPlanetRadius();
      atmosphereRadius_ = atmosphere.getAtmosphereRadius();
      rayleighScattering_ = atmosphere.getRayleighScattering();
      rayleighScaleHeight_ = atmosphere.getRayleighScaleHeight();
      mieExtinction_ =
          atmosphere.getMieScattering() + atmosphere.getMieAbsorption();
      mieScaleHeight_ = atmosphere.getMieScaleHeight();
      ozoneAbsorption_ = atmosphere.getOzoneAbsorption();
      ozoneLayerHeight_ = atmosphere.getOzoneLayerHeight();
      ozoneLayerThickness_ = atmosphere.getOzoneLayerThickness();
      cmd.bindPipeline(
          vk::PipelineBindPoint::eCompute, flyweight_->getPipeline());
      cmd.bindDescriptorSets(
          vk::PipelineBindPoint::eCompute,
          flyweight_->getPipelineLayout(),
          0,
          {getComputeDescriptorSet()},
          {});
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
      cmd.dispatch(width_ / 8u, height_ / 8u, 1u);
      return true;
    } else {
      return false;
    }
  }

  GpuImage TransmittanceLut::createImage() {
    auto createInfo = GpuImageCreateInfo{};
    createInfo.image.imageType = vk::ImageType::e2D;
    createInfo.image.format = vk::Format::eR16G16B16A16Unorm;
    createInfo.image.extent.width = width_;
    createInfo.image.extent.height = height_;
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

  vk::UniqueImageView TransmittanceLut::createImageView() {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = image_.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = vk::Format::eR16G16B16A16Unorm;
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    return flyweight_->getContext()->getDevice().createImageViewUnique(
        createInfo);
  }

  vk::UniqueDescriptorPool TransmittanceLut::createDescriptorPool() {
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
        {vk::DescriptorType::eUniformBuffer, 1},
        {vk::DescriptorType::eStorageImage, 1},
        {vk::DescriptorType::eCombinedImageSampler, 1}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = 2;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    return flyweight_->getContext()->getDevice().createDescriptorPoolUnique(
        createInfo);
  }

  std::vector<vk::DescriptorSet> TransmittanceLut::allocateDescriptorSets() {
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

  void TransmittanceLut::updateDescriptorSets() {
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
    transmittanceInfo.imageView = *imageView_;
    transmittanceInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto transmittanceWrite = vk::WriteDescriptorSet{};
    transmittanceWrite.dstSet = getComputeDescriptorSet();
    transmittanceWrite.dstBinding = 1;
    transmittanceWrite.dstArrayElement = 0;
    transmittanceWrite.descriptorCount = 1;
    transmittanceWrite.descriptorType = vk::DescriptorType::eStorageImage;
    transmittanceWrite.pImageInfo = &transmittanceInfo;
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
        {bufferWrite, transmittanceWrite, renderWrite}, {});
  }
} // namespace imp