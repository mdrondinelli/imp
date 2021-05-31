#include "SkyViewLut.h"

#include <fstream>
#include <vector>

#include "Scene.h"
#include "TransmittanceLut.h"

namespace imp {
  SkyViewLut::Flyweight::Flyweight(GpuContext &context):
      context_{&context},
      imageDescriptorSetLayout_{createImageDescriptorSetLayout()},
      textureDescriptorSetLayout_{createTextureDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()} {}

  GpuContext &SkyViewLut::Flyweight::getContext() const noexcept {
    return *context_;
  }

  vk::DescriptorSetLayout
  SkyViewLut::Flyweight::getImageDescriptorSetLayout() const noexcept {
    return imageDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SkyViewLut::Flyweight::getTextureDescriptorSetLayout() const noexcept {
    return textureDescriptorSetLayout_;
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
  SkyViewLut::Flyweight::createImageDescriptorSetLayout() {
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
  SkyViewLut::Flyweight::createTextureDescriptorSetLayout() {
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
    auto setLayouts =
        std::array{textureDescriptorSetLayout_, imageDescriptorSetLayout_};
    auto pushConstantRange = vk::PushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 96;
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = static_cast<std::uint32_t>(setLayouts.size());
    createInfo.pSetLayouts = setLayouts.data();
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
      std::shared_ptr<Flyweight const> flyweight, Vector2u const &size):
      flyweight_{std::move(flyweight)},
      size_{size},
      image_{createImage()},
      imageView_{createImageView()},
      descriptorPool_{createDescriptorPool()},
      descriptorSets_{allocateDescriptorSets()} {
    updateDescriptorSets();
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

  vk::DescriptorSet SkyViewLut::getImageDescriptorSet() const noexcept {
    return descriptorSets_[0];
  }

  vk::DescriptorSet SkyViewLut::getTextureDescriptorSet() const noexcept {
    return descriptorSets_[1];
  }

  void SkyViewLut::compute(
      vk::CommandBuffer cmd,
      Scene const &scene,
      TransmittanceLut const &transmittanceLut) {
    cmd.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getPipeline());
    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getPipelineLayout(),
        0,
        {transmittanceLut.getTextureDescriptorSet(), getImageDescriptorSet()},
        {});
    auto pushConstants = std::array<char, 96>{};
    auto pushConstantPtr = &pushConstants[0];
    auto push = [&](auto const &object) {
      std::memcpy(pushConstantPtr, &object, sizeof(object));
      pushConstantPtr += sizeof(object);
    };
    auto &camera = *scene.getCamera();
    auto &atmosphere = *scene.getAtmosphere();
    auto &sun = *scene.getSunLight();
    push(atmosphere.getRayleighScattering());
    push(atmosphere.getMieScattering());
    push(atmosphere.getOzoneAborption());
    push(atmosphere.getMieAbsorption());
    push(sun.getIrradiance());
    push(atmosphere.getPlanetRadius());
    push(sun.getDirection());
    push(atmosphere.getAtmosphereRadius());
    push(atmosphere.getRayleighScaleHeight());
    push(atmosphere.getMieScaleHeight());
    push(atmosphere.getMieG());
    push(atmosphere.getOzoneHeightCenter());
    push(atmosphere.getOzoneHeightRange());
    push(camera.getTransform().getTranslation()[1]);
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
    return GpuImage{flyweight_->getContext(), createInfo};
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
    return flyweight_->getContext().getDevice().createImageViewUnique(viewInfo);
  }

  vk::UniqueDescriptorPool SkyViewLut::createDescriptorPool() {
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

  std::vector<vk::DescriptorSet> SkyViewLut::allocateDescriptorSets() {
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

  void SkyViewLut::updateDescriptorSets() {
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
    flyweight_->getContext().getDevice().updateDescriptorSets(
        {imageWrite, textureWrite}, {});
  }
} // namespace imp