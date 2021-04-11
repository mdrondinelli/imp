#include "atmosphere_system.h"

namespace imp {
  vk::UniqueDescriptorSetLayout
  atmosphere_system::create_lut_descriptor_set_layout(
      atmosphere_system_create_info const &create_info) {
    auto binding = vk::DescriptorSetLayoutBinding{};
    binding.binding = 0;
    binding.descriptorType = vk::DescriptorType::eStorageImage;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto info = vk::DescriptorSetLayoutCreateInfo{};
    info.bindingCount = 1;
    info.pBindings = &binding;
    return create_info.device.createDescriptorSetLayoutUnique(info);
  }

  vk::UniquePipelineLayout atmosphere_system::create_scattering_pipeline_layout(
      atmosphere_system_create_info const &create_info) {
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
  }

  vk::UniquePipelineLayout
  atmosphere_system::create_optical_depth_pipeline_layout(
      atmosphere_system_create_info const &create_info) {
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
    return create_scattering_pipeline_layout(create_info);
  }

  vk::UniquePipeline atmosphere_system::create_scattering_pipeline() {
#if 0
    auto module = create_shader_module("./res/scattering.spv");
    auto pipeline_info = vk::ComputePipelineCreateInfo{};
    pipeline_info.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipeline_info.stage.module = *module;
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = *scattering_pipeline_layout_;
    pipeline_info.basePipelineIndex = -1;
    return logical_device_.handle
        ->createComputePipelineUnique({}, pipeline_info)
        .value;
#endif
  }

  scattering_lut atmosphere_system::create_scattering_lut(
      atmosphere_system_create_info const &create_info,
      gpu_allocator &allocator) {
    auto info = scattering_lut_create_info{};
    info.size[0] = 32;
    info.size[1] = 256;
    info.size[2] = 256;
    info.graphics_family = create_info.graphics_family;
    info.compute_family = create_info.compute_family;
    return scattering_lut{info, allocator};
  }

  optical_depth_lut atmosphere_system::create_optical_depth_lut(
      atmosphere_system_create_info const &create_info,
      gpu_allocator &allocator) {
    auto info = optical_depth_lut_create_info{};
    info.size[0] = 512;
    info.size[1] = 512;
    info.graphics_family = create_info.graphics_family;
    info.compute_family = create_info.compute_family;
    return optical_depth_lut{info, allocator};
  }
} // namespace imp