#include "Atmosphere.h"

#include <fstream>

namespace imp {
  namespace {
    constexpr auto const OPTICAL_DEPTH_IDX = 0u;
    constexpr auto const SCATTERING_IDX = 1u;
  } // namespace

  Atmosphere::Atmosphere(AtmosphereCreateInfo const &createInfo):
      context_{createInfo.context},
      planetRadius_{createInfo.planetRadius},
      atmosphereRadius_{createInfo.atmosphereRadius},
      scatteringCoefficients_{createInfo.scatteringCoefficients},
      commandPool_{createCommandPool()},
      lutDescriptorSetLayout_{createLutDescriptorSetLayout()},
      opticalDepthPipelineLayout_{createOpticalDepthPipelineLayout()},
      scatteringPipelineLayout_{createScatteringPipelineLayout()},
      opticalDepthPipeline_{createOpticalDepthPipeline()},
      scatteringPipeline_{createScatteringPipeline()},
      descriptorPool_{createDescriptorPool()},
      descriptorSets_(allocateDescriptorSets()),
      opticalDepthLut_{createOpticalDepthLut(createInfo.opticalDepthLutSize)},
      scatteringLut_{createScatteringLut(createInfo.scatteringLutSize)} {
    updateDescriptorSets();
    updateOpticalDepthLut();
    updateScatteringLut();
    context_->getDevice().resetCommandPool(*commandPool_);
  }

  Vector4f const &Atmosphere::getScatteringCoefficients() const noexcept {
    return scatteringCoefficients_;
  }

  float Atmosphere::getPlanetRadius() const noexcept {
    return planetRadius_;
  }

  float Atmosphere::getAtmosphereRadius() const noexcept {
    return atmosphereRadius_;
  }

  ScatteringLut const &Atmosphere::getScatteringLut() const noexcept {
    return scatteringLut_;
  }

  OpticalDepthLut const &Atmosphere::getOpticalDepthLut() const noexcept {
    return opticalDepthLut_;
  }

  vk::UniqueCommandPool Atmosphere::createCommandPool() {
    auto info = vk::CommandPoolCreateInfo{};
    info.flags = vk::CommandPoolCreateFlagBits::eTransient;
    info.queueFamilyIndex = context_->getGraphicsFamily();
    return context_->getDevice().createCommandPoolUnique(info);
  }

  vk::UniqueDescriptorSetLayout Atmosphere::createLutDescriptorSetLayout() {
    auto binding = vk::DescriptorSetLayoutBinding{};
    binding.binding = 0;
    binding.descriptorType = vk::DescriptorType::eStorageImage;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto info = vk::DescriptorSetLayoutCreateInfo{};
    info.bindingCount = 1;
    info.pBindings = &binding;
    return context_->getDevice().createDescriptorSetLayoutUnique(info);
  }

  vk::UniquePipelineLayout Atmosphere::createOpticalDepthPipelineLayout() {
#if 0
    auto set_layouts = std::array{*lut_descriptor_set_layout_};
    auto push_constant_range = vk::PushConstantRange{};
    push_constant_range.stageFlags = vk::ShaderStageFlagBits::eCompute;
    push_constant_range.offset = 0;
    push_constant_range.size = 28;
    auto info = vk::PipelineLayoutCreateInfo{};
    info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &push_constant_range;
    return create_info.device.createPipelineLayoutUnique(info);
#endif
    return createScatteringPipelineLayout();
  }

  vk::UniquePipelineLayout Atmosphere::createScatteringPipelineLayout() {
    auto set_layouts = std::array{*lutDescriptorSetLayout_};
    auto push_constant_range = vk::PushConstantRange{};
    push_constant_range.stageFlags = vk::ShaderStageFlagBits::eCompute;
    push_constant_range.offset = 0;
    push_constant_range.size = 28;
    auto info = vk::PipelineLayoutCreateInfo{};
    info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &push_constant_range;
    return context_->getDevice().createPipelineLayoutUnique(info);
  }

  vk::UniquePipeline Atmosphere::createOpticalDepthPipeline() {
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./res/optical_depth.spv", std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto code = std::vector<char>(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0, std::ios::beg);
    ifs.read(code.data(), code.size());
    ifs.close();
    auto module_info = vk::ShaderModuleCreateInfo{};
    module_info.codeSize = code.size();
    module_info.pCode = reinterpret_cast<uint32_t *>(code.data());
    auto module = context_->getDevice().createShaderModuleUnique(module_info);
    auto pipeline_info = vk::ComputePipelineCreateInfo{};
    pipeline_info.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipeline_info.stage.module = *module;
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = *opticalDepthPipelineLayout_;
    pipeline_info.basePipelineIndex = -1;
    return context_->getDevice()
        .createComputePipelineUnique({}, pipeline_info)
        .value;
  }

  vk::UniquePipeline Atmosphere::createScatteringPipeline() {
    auto ifs = std::ifstream{};
    ifs.exceptions(std::ios::badbit | std::ios::failbit);
    ifs.open("./res/scattering.spv", std::ios::binary);
    ifs.seekg(0, std::ios::end);
    auto code = std::vector<char>(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0, std::ios::beg);
    ifs.read(code.data(), code.size());
    ifs.close();
    auto module_info = vk::ShaderModuleCreateInfo{};
    module_info.codeSize = code.size();
    module_info.pCode = reinterpret_cast<uint32_t *>(code.data());
    auto module = context_->getDevice().createShaderModuleUnique(module_info);
    auto pipeline_info = vk::ComputePipelineCreateInfo{};
    pipeline_info.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipeline_info.stage.module = *module;
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = *scatteringPipelineLayout_;
    pipeline_info.basePipelineIndex = -1;
    return context_->getDevice()
        .createComputePipelineUnique({}, pipeline_info)
        .value;
  }

  vk::UniqueDescriptorPool Atmosphere::createDescriptorPool() {
    auto pool_sizes = std::vector<vk::DescriptorPoolSize>{
        // opticalDepthLut
        {vk::DescriptorType::eStorageImage, 1},
        // scatteringLut
        {vk::DescriptorType::eStorageImage, 1}};
    auto info = vk::DescriptorPoolCreateInfo{};
    info.maxSets = 2;
    info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    info.pPoolSizes = pool_sizes.data();
    return context_->getDevice().createDescriptorPoolUnique(info);
  }

  std::vector<vk::DescriptorSet> Atmosphere::allocateDescriptorSets() {
    auto set_layouts =
        std::array{*lutDescriptorSetLayout_, *lutDescriptorSetLayout_};
    auto info = vk::DescriptorSetAllocateInfo{};
    info.descriptorPool = *descriptorPool_;
    info.descriptorSetCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    return context_->getDevice().allocateDescriptorSets(info);
  }

  OpticalDepthLut Atmosphere::createOpticalDepthLut(Vector2u const &size) {
    auto createInfo = OpticalDepthLutCreateInfo{};
    createInfo.context = context_;
    createInfo.size = size;
    return OpticalDepthLut{createInfo};
  }

  ScatteringLut Atmosphere::createScatteringLut(Vector3u const &size) {
    auto createInfo = ScatteringLutCreateInfo{};
    createInfo.context = context_;
    createInfo.size = size;
    return ScatteringLut{createInfo};
  }

  void Atmosphere::updateDescriptorSets() {
    auto optical_depth_info = vk::DescriptorImageInfo{};
    optical_depth_info.imageView = opticalDepthLut_.getImageView();
    optical_depth_info.imageLayout = vk::ImageLayout::eGeneral;
    auto optical_depth_write = vk::WriteDescriptorSet{};
    optical_depth_write.dstSet = descriptorSets_[OPTICAL_DEPTH_IDX];
    optical_depth_write.dstBinding = 0;
    optical_depth_write.dstArrayElement = 0;
    optical_depth_write.descriptorCount = 1;
    optical_depth_write.descriptorType = vk::DescriptorType::eStorageImage;
    optical_depth_write.pImageInfo = &optical_depth_info;
    auto scattering_info = vk::DescriptorImageInfo{};
    scattering_info.imageView = scatteringLut_.getImageView();
    scattering_info.imageLayout = vk::ImageLayout::eGeneral;
    auto scattering_write = vk::WriteDescriptorSet{};
    scattering_write.dstSet = descriptorSets_[SCATTERING_IDX];
    scattering_write.dstBinding = 0;
    scattering_write.dstArrayElement = 0;
    scattering_write.descriptorCount = 1;
    scattering_write.descriptorType = vk::DescriptorType::eStorageImage;
    scattering_write.pImageInfo = &scattering_info;
    context_->getDevice().updateDescriptorSets(
        {optical_depth_write, scattering_write}, {});
  }

  void Atmosphere::updateOpticalDepthLut() {
    auto alloc_info = vk::CommandBufferAllocateInfo{};
    alloc_info.commandPool = *commandPool_;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandBufferCount = 1;
    auto command_buffer = std::move(
        context_->getDevice().allocateCommandBuffersUnique(alloc_info)[0]);
    auto begin_info = vk::CommandBufferBeginInfo{};
    begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    command_buffer->begin(begin_info);
    auto layout_barrier = vk::ImageMemoryBarrier{};
    layout_barrier.srcAccessMask = {};
    layout_barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
    layout_barrier.oldLayout = vk::ImageLayout::eUndefined;
    layout_barrier.newLayout = vk::ImageLayout::eGeneral;
    layout_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layout_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layout_barrier.image = opticalDepthLut_.getImage();
    layout_barrier.subresourceRange.aspectMask =
        vk::ImageAspectFlagBits::eColor;
    layout_barrier.subresourceRange.baseMipLevel = 0;
    layout_barrier.subresourceRange.levelCount = 1;
    layout_barrier.subresourceRange.baseArrayLayer = 0;
    layout_barrier.subresourceRange.layerCount = 1;
    command_buffer->pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        layout_barrier);
    command_buffer->bindPipeline(
        vk::PipelineBindPoint::eCompute, *opticalDepthPipeline_);
    command_buffer->bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        *opticalDepthPipelineLayout_,
        0,
        descriptorSets_[OPTICAL_DEPTH_IDX],
        {});
    auto integration_steps = 64;
    auto push_constants = std::array<char, 28>{};
    std::memcpy(&push_constants[0], &scatteringCoefficients_, 16);
    std::memcpy(&push_constants[16], &planetRadius_, 4);
    std::memcpy(&push_constants[20], &atmosphereRadius_, 4);
    std::memcpy(&push_constants[24], &integration_steps, 4);
    command_buffer->pushConstants(
        *opticalDepthPipelineLayout_,
        vk::ShaderStageFlagBits::eCompute,
        0,
        static_cast<uint32_t>(push_constants.size()),
        push_constants.data());
    auto group_counts = opticalDepthLut_.getSize() / 8;
    command_buffer->dispatch(group_counts[0], group_counts[1], 1);
    layout_barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
    layout_barrier.dstAccessMask = {};
    layout_barrier.oldLayout = vk::ImageLayout::eGeneral;
    layout_barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    command_buffer->pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        {},
        {},
        {},
        layout_barrier);
    command_buffer->end();
    auto submit_info = vk::SubmitInfo{};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &*command_buffer;
    context_->getGraphicsQueue().submit(submit_info);
    context_->getGraphicsQueue().waitIdle();
  }

  void Atmosphere::updateScatteringLut() {
    auto alloc_info = vk::CommandBufferAllocateInfo{};
    alloc_info.commandPool = *commandPool_;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandBufferCount = 1;
    auto command_buffer = std::move(
        context_->getDevice().allocateCommandBuffersUnique(alloc_info)[0]);
    auto begin_info = vk::CommandBufferBeginInfo{};
    begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    command_buffer->begin(begin_info);
    auto layout_barrier = vk::ImageMemoryBarrier{};
    layout_barrier.srcAccessMask = {};
    layout_barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
    layout_barrier.oldLayout = vk::ImageLayout::eUndefined;
    layout_barrier.newLayout = vk::ImageLayout::eGeneral;
    layout_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layout_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layout_barrier.image = scatteringLut_.getImage();
    layout_barrier.subresourceRange.aspectMask =
        vk::ImageAspectFlagBits::eColor;
    layout_barrier.subresourceRange.baseMipLevel = 0;
    layout_barrier.subresourceRange.levelCount = 1;
    layout_barrier.subresourceRange.baseArrayLayer = 0;
    layout_barrier.subresourceRange.layerCount = 1;
    command_buffer->pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eComputeShader,
        {},
        {},
        {},
        layout_barrier);
    command_buffer->bindPipeline(
        vk::PipelineBindPoint::eCompute, *scatteringPipeline_);
    command_buffer->bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        *scatteringPipelineLayout_,
        0,
        descriptorSets_[SCATTERING_IDX],
        {});
    auto integration_steps = 64;
    auto push_constants = std::array<char, 28>{};
    std::memcpy(&push_constants[0], &scatteringCoefficients_, 16);
    std::memcpy(&push_constants[16], &planetRadius_, 4);
    std::memcpy(&push_constants[20], &atmosphereRadius_, 4);
    std::memcpy(&push_constants[24], &integration_steps, 4);
    command_buffer->pushConstants(
        *scatteringPipelineLayout_,
        vk::ShaderStageFlagBits::eCompute,
        0,
        static_cast<uint32_t>(push_constants.size()),
        push_constants.data());
    auto group_counts = scatteringLut_.getSize() / 4;
    command_buffer->dispatch(group_counts[0], group_counts[1], group_counts[2]);
    layout_barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
    layout_barrier.dstAccessMask = {};
    layout_barrier.oldLayout = vk::ImageLayout::eGeneral;
    layout_barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    command_buffer->pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        {},
        {},
        {},
        layout_barrier);
    command_buffer->end();
    auto submit_info = vk::SubmitInfo{};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &*command_buffer;
    context_->getGraphicsQueue().submit(submit_info);
    context_->getGraphicsQueue().waitIdle();
  }
} // namespace imp