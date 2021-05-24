#include "TransmittanceLut.h"

namespace imp {
  TransmittanceLut::TransmittanceLut(
      std::shared_ptr<TransmittanceLutFlyweight const> flyweight,
      Vector2u const &size):
      flyweight_{std::move(flyweight)},
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

  void TransmittanceLut::compute(
      vk::CommandBuffer cmd, Scene const &scene) {
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
      auto pushConstants = std::array<char, 36>{};
      std::memcpy(&pushConstants[0], &rayleighExtinction_, 12);
      std::memcpy(&pushConstants[12], &mieExtinction_, 4);
      std::memcpy(&pushConstants[16], &ozoneExtinction_, 12);
      std::memcpy(&pushConstants[28], &planetRadius_, 4);
      std::memcpy(&pushConstants[32], &atmosphereRadius_, 4);
      auto groupCountX = size_[0] / 8u;
      auto groupCountY = size_[1] / 8u;
      cmd.bindPipeline(
          vk::PipelineBindPoint::eCompute, flyweight_->getPipeline());
      cmd.bindDescriptorSets(
          vk::PipelineBindPoint::eCompute,
          flyweight_->getPipelineLayout(),
          0,
          {getImageDescriptorSet()},
          {});
      cmd.pushConstants(
          flyweight_->getPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          static_cast<std::uint32_t>(pushConstants.size()),
          pushConstants.data());
      cmd.dispatch(groupCountX, groupCountY, 1u);
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
    return GpuImage{flyweight_->getContext(), createInfo};
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
    return flyweight_->getContext().getDevice().createImageViewUnique(
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
    return flyweight_->getContext().getDevice().createDescriptorPoolUnique(
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
    return flyweight_->getContext().getDevice().allocateDescriptorSets(
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
    flyweight_->getContext().getDevice().updateDescriptorSets(
        {imageWrite, textureWrite}, {});
  }
} // namespace imp