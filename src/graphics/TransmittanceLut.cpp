#include "TransmittanceLut.h"

#include <fstream>
#include <vector>

#include "../core/GpuContext.h"
#include "Scene.h"

namespace imp {
  TransmittanceLut::Flyweight::Flyweight(GpuContext *context):
      context_{context},
      imageDescriptorSetLayout_{createImageDescriptorSetLayout()},
      textureDescriptorSetLayout_{createTextureDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()} {}

  GpuContext *TransmittanceLut::Flyweight::getContext() const noexcept {
    return context_;
  }

  vk::DescriptorSetLayout
  TransmittanceLut::Flyweight::getImageDescriptorSetLayout() const noexcept {
    return imageDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  TransmittanceLut::Flyweight::getTextureDescriptorSetLayout() const noexcept {
    return textureDescriptorSetLayout_;
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
  TransmittanceLut::Flyweight::createImageDescriptorSetLayout() {
    auto binding = GpuDescriptorSetLayoutBinding{};
    binding.descriptorType = vk::DescriptorType::eStorageImage;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindingCount = 1;
    createInfo.bindings = &binding;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  TransmittanceLut::Flyweight::createTextureDescriptorSetLayout() {
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
    auto pushConstantRange = vk::PushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 52;
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &imageDescriptorSetLayout_;
    createInfo.pushConstantRangeCount = 1;
    createInfo.pPushConstantRanges = &pushConstantRange;
    return context_->getDevice().createPipelineLayoutUnique(createInfo);
  }

  vk::UniquePipeline TransmittanceLut::Flyweight::createPipeline() {
    auto code = std::vector<char>{};
    auto in = std::ifstream{};
    in.exceptions(std::ios::badbit | std::ios::failbit);
    in.open("./res/Transmittance.spv", std::ios::binary);
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
      Vector2u const &size):
      flyweight_{flyweight},
      size_{size},
      image_{createImage()},
      imageView_{createImageView()},
      descriptorPool_{createDescriptorPool()},
      descriptorSets_{allocateDescriptorSets()},
      planetRadius_{0.0f},
      atmosphereRadius_{0.0f},
      rayleighExtinction_{0.0f},
      rayleighScaleHeight_{0.0f},
      mieExtinction_{0.0f},
      mieScaleHeight_{0.0f},
      ozoneExtinction_{0.0f},
      ozoneHeightCenter_{0.0f},
      ozoneHeightRange_{0.0f} {
    updateDescriptorSets();
  }

  Vector2u const &TransmittanceLut::getSize() const noexcept {
    return size_;
  }

  vk::Image TransmittanceLut::getImage() const noexcept {
    return image_.get();
  }

  vk::ImageView TransmittanceLut::getImageView() const noexcept {
    return *imageView_;
  }

  vk::DescriptorSet TransmittanceLut::getImageDescriptorSet() const noexcept {
    return descriptorSets_[0];
  }

  vk::DescriptorSet TransmittanceLut::getTextureDescriptorSet() const noexcept {
    return descriptorSets_[1];
  }

  bool TransmittanceLut::compute(vk::CommandBuffer cmd, Scene const &scene) {
    auto &atmosphere = *scene.getAtmosphere();
    if (planetRadius_ != atmosphere.getPlanetRadius() ||
        atmosphereRadius_ != atmosphere.getAtmosphereRadius() ||
        rayleighExtinction_ != atmosphere.getRayleighScattering() ||
        rayleighScaleHeight_ != atmosphere.getRayleighScaleHeight() ||
        mieExtinction_ !=
            atmosphere.getMieScattering() + atmosphere.getMieAbsorption() ||
        mieScaleHeight_ != atmosphere.getMieScaleHeight() ||
        ozoneExtinction_ != atmosphere.getOzoneAborption() ||
        ozoneHeightCenter_ != atmosphere.getOzoneHeightCenter() ||
        ozoneHeightRange_ != atmosphere.getOzoneHeightRange()) {
      planetRadius_ = atmosphere.getPlanetRadius();
      atmosphereRadius_ = atmosphere.getAtmosphereRadius();
      rayleighExtinction_ = atmosphere.getRayleighScattering();
      rayleighScaleHeight_ = atmosphere.getRayleighScaleHeight();
      mieExtinction_ =
          atmosphere.getMieScattering() + atmosphere.getMieAbsorption();
      mieScaleHeight_ = atmosphere.getMieScaleHeight();
      ozoneExtinction_ = atmosphere.getOzoneAborption();
      ozoneHeightCenter_ = atmosphere.getOzoneHeightCenter();
      ozoneHeightRange_ = atmosphere.getOzoneHeightRange();
      cmd.bindPipeline(
          vk::PipelineBindPoint::eCompute, flyweight_->getPipeline());
      cmd.bindDescriptorSets(
          vk::PipelineBindPoint::eCompute,
          flyweight_->getPipelineLayout(),
          0,
          {getImageDescriptorSet()},
          {});
      auto pushConstants = std::array<char, 52>{};
      std::memcpy(&pushConstants[0], &rayleighExtinction_, 12);
      std::memcpy(&pushConstants[12], &rayleighScaleHeight_, 4);
      std::memcpy(&pushConstants[16], &ozoneExtinction_, 12);
      std::memcpy(&pushConstants[28], &ozoneHeightCenter_, 4);
      std::memcpy(&pushConstants[32], &ozoneHeightRange_, 4);
      std::memcpy(&pushConstants[36], &mieExtinction_, 4);
      std::memcpy(&pushConstants[40], &mieScaleHeight_, 4);
      std::memcpy(&pushConstants[44], &planetRadius_, 4);
      std::memcpy(&pushConstants[48], &atmosphereRadius_, 4);
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
      return true;
    } else {
      return false;
    }
  }

  GpuImage TransmittanceLut::createImage() {
    auto createInfo = GpuImageCreateInfo{};
    createInfo.image.imageType = vk::ImageType::e2D;
    createInfo.image.format = vk::Format::eR16G16B16A16Unorm;
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
        flyweight_->getImageDescriptorSetLayout(),
        flyweight_->getTextureDescriptorSetLayout()};
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = *descriptorPool_;
    allocateInfo.descriptorSetCount =
        static_cast<std::uint32_t>(setLayouts.size());
    allocateInfo.pSetLayouts = setLayouts.data();
    return flyweight_->getContext()->getDevice().allocateDescriptorSets(
        allocateInfo);
  }

  void TransmittanceLut::updateDescriptorSets() {
    auto imageImageInfo = vk::DescriptorImageInfo{};
    imageImageInfo.imageView = *imageView_;
    imageImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto imageWrite = vk::WriteDescriptorSet{};
    imageWrite.dstSet = getImageDescriptorSet();
    imageWrite.dstBinding = 0;
    imageWrite.dstArrayElement = 0;
    imageWrite.descriptorCount = 1;
    imageWrite.descriptorType = vk::DescriptorType::eStorageImage;
    imageWrite.pImageInfo = &imageImageInfo;
    auto textureImageInfo = vk::DescriptorImageInfo{};
    textureImageInfo.sampler = flyweight_->getSampler();
    textureImageInfo.imageView = *imageView_;
    textureImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto textureWrite = vk::WriteDescriptorSet{};
    textureWrite.dstSet = getTextureDescriptorSet();
    textureWrite.dstBinding = 0;
    textureWrite.dstArrayElement = 0;
    textureWrite.descriptorCount = 1;
    textureWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    textureWrite.pImageInfo = &textureImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {imageWrite, textureWrite}, {});
  }
} // namespace imp