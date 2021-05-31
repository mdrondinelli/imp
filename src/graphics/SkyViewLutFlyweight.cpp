#include "SkyViewLutFlyweight.h"

#include <fstream>

namespace imp {
  SkyViewLutFlyweight::SkyViewLutFlyweight(GpuContext &context):
      context_{&context},
      imageDescriptorSetLayout_{createImageDescriptorSetLayout()},
      textureDescriptorSetLayout_{createTextureDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()} {}

  GpuContext &SkyViewLutFlyweight::getContext() const noexcept {
    return *context_;
  }

  vk::DescriptorSetLayout
  SkyViewLutFlyweight::getImageDescriptorSetLayout() const noexcept {
    return imageDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SkyViewLutFlyweight::getTextureDescriptorSetLayout() const noexcept {
    return textureDescriptorSetLayout_;
  }

  vk::PipelineLayout SkyViewLutFlyweight::getPipelineLayout() const noexcept {
    return *pipelineLayout_;
  }

  vk::Pipeline SkyViewLutFlyweight::getPipeline() const noexcept {
    return *pipeline_;
  }

  vk::Sampler SkyViewLutFlyweight::getSampler() const noexcept {
    return sampler_;
  }

  vk::DescriptorSetLayout
  SkyViewLutFlyweight::createImageDescriptorSetLayout() {
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
  SkyViewLutFlyweight::createTextureDescriptorSetLayout() {
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

  vk::UniquePipelineLayout SkyViewLutFlyweight::createPipelineLayout() {
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

  vk::UniquePipeline SkyViewLutFlyweight::createPipeline() {
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
    return context_->getDevice().createComputePipelineUnique({}, createInfo).value;
  }

  vk::Sampler SkyViewLutFlyweight::createSampler() {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }
} // namespace imp