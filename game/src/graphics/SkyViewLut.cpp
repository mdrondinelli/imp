//#include "SkyViewLut.h"
//
//#include <cassert>
//
//#include <fstream>
//#include <vector>
//
//#include "../system/GpuContext.h"
//#include "AtmosphereBuffer.h"
//#include "Scene.h"
//#include "TransmittanceLut.h"
//
//namespace imp {
//  SkyViewLut::Flyweight::Flyweight(GpuContext *context):
//      context_{context},
//      descriptorSetLayout_{createDescriptorSetLayout()},
//      pipelineLayout_{createPipelineLayout()},
//      pipeline_{createPipeline()},
//      sampler_{createSampler()} {}
//
//  GpuContext *SkyViewLut::Flyweight::getContext() const noexcept {
//    return context_;
//  }
//
//  vk::DescriptorSetLayout
//  SkyViewLut::Flyweight::getDescriptorSetLayout() const noexcept {
//    return descriptorSetLayout_;
//  }
//
//  vk::PipelineLayout SkyViewLut::Flyweight::getPipelineLayout() const noexcept {
//    return *pipelineLayout_;
//  }
//
//  vk::Pipeline SkyViewLut::Flyweight::getPipeline() const noexcept {
//    return *pipeline_;
//  }
//
//  vk::Sampler SkyViewLut::Flyweight::getSampler() const noexcept {
//    return sampler_;
//  }
//
//  vk::DescriptorSetLayout SkyViewLut::Flyweight::createDescriptorSetLayout() {
//    auto bufferBinding = GpuDescriptorSetLayoutBinding{};
//    bufferBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
//    bufferBinding.descriptorCount = 1;
//    bufferBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
//    auto textureBinding = GpuDescriptorSetLayoutBinding{};
//    textureBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
//    textureBinding.descriptorCount = 1;
//    textureBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
//    auto imageBinding = GpuDescriptorSetLayoutBinding{};
//    imageBinding.descriptorType = vk::DescriptorType::eStorageImage;
//    imageBinding.descriptorCount = 1;
//    imageBinding.stageFlags = vk::ShaderStageFlagBits::eCompute;
//    auto bindings = std::array{bufferBinding, textureBinding, imageBinding};
//    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
//    createInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
//    createInfo.bindings = bindings.data();
//    return context_->createDescriptorSetLayout(createInfo);
//  }
//
//  vk::UniquePipelineLayout SkyViewLut::Flyweight::createPipelineLayout() {
//    auto pushConstantRange = vk::PushConstantRange{};
//    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
//    pushConstantRange.offset = 0;
//    pushConstantRange.size = 32;
//    auto createInfo = vk::PipelineLayoutCreateInfo{};
//    createInfo.setLayoutCount = 1;
//    createInfo.pSetLayouts = &descriptorSetLayout_;
//    createInfo.pushConstantRangeCount = 1;
//    createInfo.pPushConstantRanges = &pushConstantRange;
//    return context_->getDevice().createPipelineLayoutUnique(createInfo);
//  }
//
//  vk::UniquePipeline SkyViewLut::Flyweight::createPipeline() {
//    auto ifs = std::ifstream{};
//    ifs.exceptions(std::ios::badbit | std::ios::failbit);
//    ifs.open("./data/SkyView.spv", std::ios::binary);
//    ifs.seekg(0, std::ios::end);
//    auto code = std::vector<char>{};
//    code.resize(ifs.tellg());
//    if (code.size() % 4 != 0) {
//      throw std::runtime_error{"invalid shader module"};
//    }
//    ifs.seekg(0, std::ios::beg);
//    ifs.read(code.data(), code.size());
//    ifs.close();
//    auto moduleCreateInfo = vk::ShaderModuleCreateInfo{};
//    moduleCreateInfo.codeSize = code.size();
//    moduleCreateInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
//    auto module =
//        context_->getDevice().createShaderModuleUnique(moduleCreateInfo);
//    auto createInfo = vk::ComputePipelineCreateInfo{};
//    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
//    createInfo.stage.module = *module;
//    createInfo.stage.pName = "main";
//    createInfo.layout = *pipelineLayout_;
//    createInfo.basePipelineIndex = -1;
//    return context_->getDevice()
//        .createComputePipelineUnique({}, createInfo)
//        .value;
//  }
//
//  vk::Sampler SkyViewLut::Flyweight::createSampler() {
//    auto createInfo = GpuSamplerCreateInfo{};
//    createInfo.magFilter = vk::Filter::eLinear;
//    createInfo.minFilter = vk::Filter::eLinear;
//    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
//    createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
//    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
//    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
//    return context_->createSampler(createInfo);
//  }
//
//  SkyViewLut::SkyViewLut(
//      Flyweight const *flyweight,
//      TransmittanceLut const *transmittanceLut,
//      Extent2u const &extent):
//      flyweight_{flyweight},
//      atmosphereBuffer_{transmittanceLut->getAtmosphereBuffer()},
//      transmittanceLut_{transmittanceLut},
//      extent_{extent},
//      image_{createImage()},
//      imageViews_{createImageViews()},
//      descriptorPool_{createDescriptorPool()},
//      descriptorSets_{allocateDescriptorSets()} {
//    assert(width % 8 == 0);
//    assert(height % 8 == 0);
//    updateDescriptorSets();
//  }
//
//  unsigned SkyViewLut::getFrameCount() const noexcept {
//    return atmosphereBuffer_->getFrameCount();
//  }
//
//  unsigned SkyViewLut::getFrameIndex() const noexcept {
//    return atmosphereBuffer_->getFrameIndex();
//  }
//
//  SkyViewLut::Flyweight const *SkyViewLut::getFlyweight() const noexcept {
//    return flyweight_;
//  }
//
//  AtmosphereBuffer const *SkyViewLut::getAtmosphereBuffer() const noexcept {
//    return atmosphereBuffer_;
//  }
//
//  TransmittanceLut const *SkyViewLut::getTransmittanceLut() const noexcept {
//    return transmittanceLut_;
//  }
//
//  Extent2u const &SkyViewLut::getExtent() const noexcept {
//    return extent_;
//  }
//
//  GpuImage const &SkyViewLut::getImage() const noexcept {
//    return image_;
//  }
//
//  vk::ImageView SkyViewLut::getImageView() const noexcept {
//    return *imageViews_[getFrameIndex()];
//  }
//
//  vk::ImageView SkyViewLut::getImageView(unsigned frameIndex) const noexcept {
//    return *imageViews_[frameIndex];
//  }
//
//  void SkyViewLut::compute(
//      vk::CommandBuffer cmd, DirectionalLight const &sun, float altitude) {
//    cmd.bindPipeline(
//        vk::PipelineBindPoint::eCompute, flyweight_->getPipeline());
//    cmd.bindDescriptorSets(
//        vk::PipelineBindPoint::eCompute,
//        flyweight_->getPipelineLayout(),
//        0,
//        {descriptorSets_[getFrameIndex()]},
//        {});
//    auto pushConstants = std::array<char, 32>{};
//    std::memcpy(&pushConstants[0], sun.getIrradiance().data(), 12);
//    std::memcpy(&pushConstants[16], sun.getDirection().data(), 12);
//    std::memcpy(&pushConstants[28], &altitude, 4);
//    cmd.pushConstants(
//        flyweight_->getPipelineLayout(),
//        vk::ShaderStageFlagBits::eCompute,
//        0,
//        static_cast<std::uint32_t>(pushConstants.size()),
//        pushConstants.data());
//    auto barrier = vk::ImageMemoryBarrier{};
//    barrier.srcAccessMask = {};
//    barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
//    barrier.oldLayout = vk::ImageLayout::eUndefined;
//    barrier.newLayout = vk::ImageLayout::eGeneral;
//    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//    barrier.image = image_.get();
//    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
//    barrier.subresourceRange.baseMipLevel = 0;
//    barrier.subresourceRange.levelCount = 1;
//    barrier.subresourceRange.baseArrayLayer = 0;
//    barrier.subresourceRange.layerCount = 1;
//    cmd.pipelineBarrier(
//        vk::PipelineStageFlagBits::eTopOfPipe,
//        vk::PipelineStageFlagBits::eComputeShader,
//        {},
//        {},
//        {},
//        barrier);
//    cmd.dispatch(extent_.width / 8u, extent_.height / 8u, 1u);
//  }
//
//  GpuImage SkyViewLut::createImage() {
//    auto image = vk::ImageCreateInfo{};
//    image.imageType = vk::ImageType::e2D;
//    image.format = vk::Format::eR16G16B16A16Sfloat;
//    image.extent = static_cast<vk::Extent3D>(extent_);
//    image.mipLevels = 1;
//    image.arrayLayers = getFrameCount();
//    image.samples = vk::SampleCountFlagBits::e1;
//    image.tiling = vk::ImageTiling::eOptimal;
//    image.usage =
//        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage;
//    image.sharingMode = vk::SharingMode::eExclusive;
//    image.initialLayout = vk::ImageLayout::eUndefined;
//    auto allocation = VmaAllocationCreateInfo{};
//    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
//    return GpuImage{
//        flyweight_->getContext()->getAllocator(), image, allocation};
//  }
//
//  std::vector<vk::UniqueImageView> SkyViewLut::createImageViews() {
//    auto createInfo = vk::ImageViewCreateInfo{};
//    createInfo.image = image_.get();
//    createInfo.viewType = vk::ImageViewType::e2D;
//    createInfo.format = vk::Format::eR16G16B16A16Sfloat;
//    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
//    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
//    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
//    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
//    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
//    createInfo.subresourceRange.baseMipLevel = 0;
//    createInfo.subresourceRange.levelCount = 1;
//    createInfo.subresourceRange.baseArrayLayer = 0;
//    createInfo.subresourceRange.layerCount = 1;
//    auto ret = std::vector<vk::UniqueImageView>{};
//    ret.reserve(getFrameCount());
//    for (auto i = 0u; i < getFrameCount(); ++i) {
//      createInfo.subresourceRange.baseArrayLayer = i;
//      ret.emplace_back(
//          flyweight_->getContext()->getDevice().createImageViewUnique(
//              createInfo));
//    }
//    return ret;
//  }
//
//  vk::UniqueDescriptorPool SkyViewLut::createDescriptorPool() {
//    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
//        {vk::DescriptorType::eUniformBuffer, getFrameCount()},
//        {vk::DescriptorType::eStorageImage, getFrameCount()},
//        {vk::DescriptorType::eCombinedImageSampler, getFrameCount()}};
//    auto createInfo = vk::DescriptorPoolCreateInfo{};
//    createInfo.maxSets = getFrameCount();
//    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
//    createInfo.pPoolSizes = poolSizes.data();
//    return flyweight_->getContext()->getDevice().createDescriptorPoolUnique(
//        createInfo);
//  }
//
//  std::vector<vk::DescriptorSet> SkyViewLut::allocateDescriptorSets() {
//    auto setLayouts = std::vector<vk::DescriptorSetLayout>(
//        getFrameCount(), flyweight_->getDescriptorSetLayout());
//    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
//    allocateInfo.descriptorPool = *descriptorPool_;
//    allocateInfo.descriptorSetCount =
//        static_cast<std::uint32_t>(setLayouts.size());
//    allocateInfo.pSetLayouts = setLayouts.data();
//    return flyweight_->getContext()->getDevice().allocateDescriptorSets(
//        allocateInfo);
//  }
//
//  void SkyViewLut::updateDescriptorSets() {
//    auto bufferInfo = vk::DescriptorBufferInfo{};
//    bufferInfo.buffer = atmosphereBuffer_->getBuffer().get();
//    bufferInfo.range = AtmosphereBuffer::SIZE;
//    auto bufferWrite = vk::WriteDescriptorSet{};
//    bufferWrite.dstBinding = 0;
//    bufferWrite.dstArrayElement = 0;
//    bufferWrite.descriptorCount = 1;
//    bufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
//    bufferWrite.pBufferInfo = &bufferInfo;
//    auto transmittanceInfo = vk::DescriptorImageInfo{};
//    transmittanceInfo.sampler = transmittanceLut_->getFlyweight()->getSampler();
//    transmittanceInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
//    auto transmittanceWrite = vk::WriteDescriptorSet{};
//    transmittanceWrite.dstBinding = 1;
//    transmittanceWrite.dstArrayElement = 0;
//    transmittanceWrite.descriptorCount = 1;
//    transmittanceWrite.descriptorType =
//        vk::DescriptorType::eCombinedImageSampler;
//    transmittanceWrite.pImageInfo = &transmittanceInfo;
//    auto skyViewInfo = vk::DescriptorImageInfo{};
//    skyViewInfo.imageLayout = vk::ImageLayout::eGeneral;
//    auto skyViewWrite = vk::WriteDescriptorSet{};
//    skyViewWrite.dstBinding = 2;
//    skyViewWrite.dstArrayElement = 0;
//    skyViewWrite.descriptorCount = 1;
//    skyViewWrite.descriptorType = vk::DescriptorType::eStorageImage;
//    skyViewWrite.pImageInfo = &skyViewInfo;
//    for (auto i = 0u; i < getFrameCount(); ++i) {
//      bufferInfo.offset = atmosphereBuffer_->getOffset(i);
//      bufferWrite.dstSet = descriptorSets_[i];
//      transmittanceInfo.imageView = transmittanceLut_->getImageView(i);
//      transmittanceWrite.dstSet = descriptorSets_[i];
//      skyViewInfo.imageView = *imageViews_[i];
//      skyViewWrite.dstSet = descriptorSets_[i];
//      flyweight_->getContext()->getDevice().updateDescriptorSets(
//          {bufferWrite, transmittanceWrite, skyViewWrite}, {});
//    }
//  }
//} // namespace imp