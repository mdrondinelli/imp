#include "TransmittanceLutFlyweight.h"

#include <filesystem>
#include <fstream>

namespace imp {
  TransmittanceLutFlyweight::TransmittanceLutFlyweight(GpuContext &context):
      context_{&context},
      imageDescriptorSetLayout_{createImageDescriptorSetLayout()},
      textureDescriptorSetLayout_{createTextureDescriptorSetLayout()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()},
      sampler_{createSampler()} {}

  GpuContext &TransmittanceLutFlyweight::getContext() const noexcept {
    return *context_;
  }

  vk::DescriptorSetLayout
  TransmittanceLutFlyweight::getImageDescriptorSetLayout() const noexcept {
    return imageDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  TransmittanceLutFlyweight::getTextureDescriptorSetLayout() const noexcept {
    return textureDescriptorSetLayout_;
  }

  vk::PipelineLayout
  TransmittanceLutFlyweight::getPipelineLayout() const noexcept {
    return *pipelineLayout_;
  }

  vk::Pipeline TransmittanceLutFlyweight::getPipeline() const noexcept {
    return *pipeline_;
  }

  vk::Sampler TransmittanceLutFlyweight::getSampler() const noexcept {
    return sampler_;
  }

  vk::DescriptorSetLayout
  TransmittanceLutFlyweight::createImageDescriptorSetLayout() {
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
  TransmittanceLutFlyweight::createTextureDescriptorSetLayout() {
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

  vk::UniquePipelineLayout TransmittanceLutFlyweight::createPipelineLayout() {
    auto pushConstantRange = vk::PushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eCompute;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 36;
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &imageDescriptorSetLayout_;
    createInfo.pushConstantRangeCount = 1;
    createInfo.pPushConstantRanges = &pushConstantRange;
    return context_->getDevice().createPipelineLayoutUnique(createInfo);
  }

  namespace {
    vk::UniqueShaderModule
    createShaderModule(GpuContext &context, std::filesystem::path const &path) {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open(path, std::ios::binary);
      in.seekg(0, std::ios::end);
      auto size = static_cast<std::size_t>(in.tellg());
      if (size % 4 != 0) {
        throw std::runtime_error{"failed to create shader module"};
      }
      code.resize(size);
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto moduleCreateInfo = vk::ShaderModuleCreateInfo{};
      moduleCreateInfo.codeSize = static_cast<std::uint32_t>(code.size());
      moduleCreateInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      return context.getDevice().createShaderModuleUnique(moduleCreateInfo);
    }
  } // namespace

  vk::UniquePipeline TransmittanceLutFlyweight::createPipeline() {
    auto module = createShaderModule(*context_, "./res/Transmittance.spv");
    auto createInfo = vk::ComputePipelineCreateInfo{};
    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    createInfo.stage.module = *module;
    createInfo.stage.pName = "main";
    createInfo.layout = *pipelineLayout_;
    createInfo.basePipelineIndex = -1;
    return context_->getDevice()
        .createComputePipelineUnique({}, createInfo).value;
  }

  vk::Sampler TransmittanceLutFlyweight::createSampler() {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }
} // namespace imp