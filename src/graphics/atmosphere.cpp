#include "atmosphere.h"

#include <fstream>

namespace imp {
  namespace {
    constexpr auto scattering_idx = size_t{0};
    constexpr auto optical_depth_idx = size_t{1};
  } // namespace

  atmosphere::atmosphere(
      gpu_context &context,
      vector4f const &scattering_coefficients,
      float planet_radius,
      float atmosphere_radius):
      context_{&context},
      scattering_coefficients_{scattering_coefficients},
      planet_radius_{planet_radius},
      atmosphere_radius_{atmosphere_radius},
      command_pool_{create_command_pool()},
      lut_descriptor_set_layout_{create_lut_descriptor_set_layout()},
      scattering_pipeline_layout_{create_scattering_pipeline_layout()},
      optical_depth_pipeline_layout_{create_optical_depth_pipeline_layout()},
      scattering_pipeline_{create_scattering_pipeline()},
      optical_depth_pipeline_{create_optical_depth_pipeline()},
      descriptor_pool_{create_descriptor_pool()},
      descriptor_sets_(allocate_descriptor_sets()),
      scattering_{context, make_vector(32, 512, 256)},
      optical_depth_{context, make_vector(512, 512)} {
    update_descriptor_sets();
    update_scattering_lut();
    update_optical_depth_lut();
    context_->device().resetCommandPool(*command_pool_);
  }

  vk::UniqueCommandPool atmosphere::create_command_pool() {
    auto info = vk::CommandPoolCreateInfo{};
    info.flags = vk::CommandPoolCreateFlagBits::eTransient;
    info.queueFamilyIndex = context_->graphics_family();
    return context_->device().createCommandPoolUnique(info);
  }

  vk::UniqueDescriptorSetLayout atmosphere::create_lut_descriptor_set_layout() {
    auto binding = vk::DescriptorSetLayoutBinding{};
    binding.binding = 0;
    binding.descriptorType = vk::DescriptorType::eStorageImage;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto info = vk::DescriptorSetLayoutCreateInfo{};
    info.bindingCount = 1;
    info.pBindings = &binding;
    return context_->device().createDescriptorSetLayoutUnique(info);
  }

  vk::UniquePipelineLayout atmosphere::create_scattering_pipeline_layout() {
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
    return context_->device().createPipelineLayoutUnique(info);
  }

  vk::UniquePipelineLayout atmosphere::create_optical_depth_pipeline_layout() {
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
    return create_scattering_pipeline_layout();
  }

  vk::UniquePipeline atmosphere::create_scattering_pipeline() {
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
    auto module = context_->device().createShaderModuleUnique(module_info);
    auto pipeline_info = vk::ComputePipelineCreateInfo{};
    pipeline_info.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipeline_info.stage.module = *module;
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = *scattering_pipeline_layout_;
    pipeline_info.basePipelineIndex = -1;
    return context_->device()
        .createComputePipelineUnique({}, pipeline_info)
        .value;
  }

  vk::UniquePipeline atmosphere::create_optical_depth_pipeline() {
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
    auto module = context_->device().createShaderModuleUnique(module_info);
    auto pipeline_info = vk::ComputePipelineCreateInfo{};
    pipeline_info.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipeline_info.stage.module = *module;
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = *optical_depth_pipeline_layout_;
    pipeline_info.basePipelineIndex = -1;
    return context_->device()
        .createComputePipelineUnique({}, pipeline_info)
        .value;
  }

  vk::UniqueDescriptorPool atmosphere::create_descriptor_pool() {
    auto pool_sizes = std::vector<vk::DescriptorPoolSize>{
        // scattering
        {vk::DescriptorType::eStorageImage, 1},
        // optical_depth
        {vk::DescriptorType::eStorageImage, 1}};
    auto info = vk::DescriptorPoolCreateInfo{};
    info.maxSets = 2;
    info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    info.pPoolSizes = pool_sizes.data();
    return context_->device().createDescriptorPoolUnique(info);
  }

  std::vector<vk::DescriptorSet> atmosphere::allocate_descriptor_sets() {
    auto set_layouts =
        std::array{*lut_descriptor_set_layout_, *lut_descriptor_set_layout_};
    auto info = vk::DescriptorSetAllocateInfo{};
    info.descriptorPool = *descriptor_pool_;
    info.descriptorSetCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    return context_->device().allocateDescriptorSets(info);
  }

  void atmosphere::update_descriptor_sets() {
    auto scattering_info = vk::DescriptorImageInfo{};
    scattering_info.imageView = scattering_.image_view();
    scattering_info.imageLayout = vk::ImageLayout::eGeneral;
    auto scattering_write = vk::WriteDescriptorSet{};
    scattering_write.dstSet = descriptor_sets_[scattering_idx];
    scattering_write.dstBinding = 0;
    scattering_write.dstArrayElement = 0;
    scattering_write.descriptorCount = 1;
    scattering_write.descriptorType = vk::DescriptorType::eStorageImage;
    scattering_write.pImageInfo = &scattering_info;
    auto optical_depth_info = vk::DescriptorImageInfo{};
    optical_depth_info.imageView = optical_depth_.image_view();
    optical_depth_info.imageLayout = vk::ImageLayout::eGeneral;
    auto optical_depth_write = vk::WriteDescriptorSet{};
    optical_depth_write.dstSet = descriptor_sets_[optical_depth_idx];
    optical_depth_write.dstBinding = 0;
    optical_depth_write.dstArrayElement = 0;
    optical_depth_write.descriptorCount = 1;
    optical_depth_write.descriptorType = vk::DescriptorType::eStorageImage;
    optical_depth_write.pImageInfo = &optical_depth_info;
    auto writes = {scattering_write, optical_depth_write};
    context_->device().updateDescriptorSets(writes, {});
  }

  void atmosphere::update_scattering_lut() {
    auto alloc_info = vk::CommandBufferAllocateInfo{};
    alloc_info.commandPool = *command_pool_;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandBufferCount = 1;
    auto command_buffer = std::move(
        context_->device().allocateCommandBuffersUnique(alloc_info)[0]);
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
    layout_barrier.image = scattering_.image();
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
        vk::PipelineBindPoint::eCompute, *scattering_pipeline_);
    command_buffer->bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        *scattering_pipeline_layout_,
        0,
        descriptor_sets_[scattering_idx],
        {});
    auto integration_steps = 64;
    auto push_constants = std::array<char, 28>{};
    std::memcpy(&push_constants[0], &scattering_coefficients_, 16);
    std::memcpy(&push_constants[16], &planet_radius_, 4);
    std::memcpy(&push_constants[20], &atmosphere_radius_, 4);
    std::memcpy(&push_constants[24], &integration_steps, 4);
    command_buffer->pushConstants(
        *scattering_pipeline_layout_,
        vk::ShaderStageFlagBits::eCompute,
        0,
        static_cast<uint32_t>(push_constants.size()),
        push_constants.data());
    auto group_counts = scattering_.size() / 4;
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
    context_->graphics_queue().submit(submit_info);
    context_->graphics_queue().waitIdle();
  }

  void atmosphere::update_optical_depth_lut() {
    auto alloc_info = vk::CommandBufferAllocateInfo{};
    alloc_info.commandPool = *command_pool_;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandBufferCount = 1;
    auto command_buffer = std::move(
        context_->device().allocateCommandBuffersUnique(alloc_info)[0]);
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
    layout_barrier.image = optical_depth_.image();
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
        vk::PipelineBindPoint::eCompute, *optical_depth_pipeline_);
    command_buffer->bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        *optical_depth_pipeline_layout_,
        0,
        descriptor_sets_[optical_depth_idx],
        {});
    auto integration_steps = 64;
    auto push_constants = std::array<char, 28>{};
    std::memcpy(&push_constants[0], &scattering_coefficients_, 16);
    std::memcpy(&push_constants[16], &planet_radius_, 4);
    std::memcpy(&push_constants[20], &atmosphere_radius_, 4);
    std::memcpy(&push_constants[24], &integration_steps, 4);
    command_buffer->pushConstants(
        *optical_depth_pipeline_layout_,
        vk::ShaderStageFlagBits::eCompute,
        0,
        static_cast<uint32_t>(push_constants.size()),
        push_constants.data());
    auto group_counts = optical_depth_.size() / 8;
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
    context_->graphics_queue().submit(submit_info);
    context_->graphics_queue().waitIdle();
  }

  scattering_lut const &atmosphere::scattering() const noexcept {
    return scattering_;
  }

  optical_depth_lut const &atmosphere::optical_depth() const noexcept {
    return optical_depth_;
  }
} // namespace imp