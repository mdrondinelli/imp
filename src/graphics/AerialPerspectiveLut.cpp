#include "AerialPerspectiveLut.h"

#include <fstream>
#include <vector>

#include "../core/GpuContext.h"
#include "Scene.h"

namespace imp {
  AerialPerspectiveLut::Flyweight::Flyweight(GpuContext &context):
      context_{&context},
      imageDescriptorSetLayout_{createImageDescriptorSetLayout()},
      textureDescriptorSetLayout_{createTextureDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()} {}

  GpuContext &AerialPerspectiveLut::Flyweight::getContext() const noexcept {
    return *context_;
  }

  vk::DescriptorSetLayout
  AerialPerspectiveLut::Flyweight::getImageDescriptorSetLayout()
      const noexcept {
    return imageDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  AerialPerspectiveLut::Flyweight::getTextureDescriptorSetLayout()
      const noexcept {
    return textureDescriptorSetLayout_;
  }

  vk::PipelineLayout
  AerialPerspectiveLut::Flyweight::getPipelineLayout() const noexcept {
    return *pipelineLayout_;
  }

  vk::Pipeline AerialPerspectiveLut::Flyweight::getPipeline() const noexcept {
    return *pipeline_;
  }

  vk::Sampler AerialPerspectiveLut::Flyweight::getSampler() const noexcept {
    return sampler_;
  }

  vk::DescriptorSetLayout
  AerialPerspectiveLut::Flyweight::createImageDescriptorSetLayout() {
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
  AerialPerspectiveLut::Flyweight::createTextureDescriptorSetLayout() {
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

  vk::UniquePipelineLayout
  AerialPerspectiveLut::Flyweight::createPipelineLayout() {
    auto setLayouts =
        std::array{textureDescriptorSetLayout_, imageDescriptorSetLayout_};
    auto pushConstantRange = vk::PushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 104;
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = static_cast<std::uint32_t>(setLayouts.size());
  }

  vk::UniquePipeline AerialPerspectiveLut::Flyweight::createPipeline() {
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./res/SkyView.spv", std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto codeSize = static_cast<std::size_t>(ifs.tellg());
    if (codeSize % 4 != 0) {
      throw std::runtime_error{"invalid shader module"};
    }
    auto code = std::vector<char>{};
    code.resize(codeSize);
    ifs.seekg(0, std::ios::beg);
    ifs.read(code.data(), code.size());
    ifs.close();
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

  vk::Sampler AerialPerspectiveLut::Flyweight::createSampler() {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  AerialPerspectiveLut::AerialPerspectiveLut(
      std::shared_ptr<Flyweight const> flyweight, Vector3u const &size):
      flyweight_{std::move(flyweight)},
      size_{size},
      image_{createImage()},
      imageView_{createImageView()} {}

  Vector3u const &AerialPerspectiveLut::getSize() const noexcept {
    return size_;
  }

  vk::DescriptorSet
  AerialPerspectiveLut::getImageDescriptorSet() const noexcept {
    return descriptorSets_[0];
  }

  vk::DescriptorSet
  AerialPerspectiveLut::getTextureDescriptorSet() const noexcept {
    return descriptorSets_[1];
  }

  void AerialPerspectiveLut::compute(
      vk::CommandBuffer cmd,
      Scene const &scene,
      TransmittanceLut const &transmittanceLut) {}

  GpuImage AerialPerspectiveLut::createImage() {
    auto createInfo = GpuImageCreateInfo{};
    createInfo.image.imageType = vk::ImageType::e3D;
    createInfo.image.format = vk::Format::eR16G16B16A16Sfloat;
    createInfo.image.extent.width = size_[0];
    createInfo.image.extent.height = size_[1];
    createInfo.image.extent.depth = size_[2];
    createInfo.image.mipLevels = 1;
    createInfo.image.arrayLayers = 1;
    createInfo.image.samples = vk::SampleCountFlagBits::e1;
    createInfo.image.tiling = vk::ImageTiling::eOptimal;
    createInfo.image.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
    createInfo.image.sharingMode = vk::SharingMode::eExclusive;
    createInfo.image.initialLayout = vk::ImageLayout::eUndefined;
    createInfo.allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{flyweight_->getContext(), createInfo};
  }

  vk::UniqueImageView AerialPerspectiveLut::createImageView() {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = image_.get();
    createInfo.viewType = vk::ImageViewType::e3D;
    createInfo.format = vk::Format::eR16G16B16A16Sfloat;
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    return flyweight_->getContext().getDevice().createImageViewUnique(
        createInfo);
  }

  vk::UniqueDescriptorPool AerialPerspectiveLut::createDescriptorPool() {
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
        {vk::DescriptorType::eStorageImage, 1},
        {vk::DescriptorType::eCombinedImageSampler, 1}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = 2;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    return flyweight_->getContext().getDevice().createDescriptorPoolUnique(
        createInfo);
  }

  std::vector<vk::DescriptorSet>
  AerialPerspectiveLut::allocateDescriptorSets() {
    auto setLayouts = std::array{
        flyweight_->getImageDescriptorSetLayout(),
        flyweight_->getTextureDescriptorSetLayout()};
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = *descriptorPool_;
    allocateInfo.descriptorSetCount =
        static_cast<std::uint32_t>(setLayouts.size());
    allocateInfo.pSetLayouts = setLayouts.data();
    return flyweight_->getContext().getDevice().allocateDescriptorSets(
        allocateInfo);
  }

  void AerialPerspectiveLut::updateDescriptorSets() {
    auto imageInfo = vk::DescriptorImageInfo{};
    imageInfo.imageView = *imageView_;
    imageInfo.imageLayout = vk::ImageLayout::eGeneral;
    auto imageWrite = vk::WriteDescriptorSet{};
    imageWrite.dstSet = getImageDescriptorSet();
    imageWrite.dstBinding = 0;
    imageWrite.dstArrayElement = 0;
    imageWrite.descriptorCount = 1;
    imageWrite.descriptorType = vk::DescriptorType::eStorageImage;
    imageWrite.pImageInfo = &imageInfo;
    auto textureInfo = vk::DescriptorImageInfo{};
    textureInfo.sampler = flyweight_->getSampler();
    textureInfo.imageView = *imageView_;
    textureInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto textureWrite = vk::WriteDescriptorSet{};
  }
} // namespace imp