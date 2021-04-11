#pragma once

#include "atmosphere_system_create_info.h"
#include "optical_depth_lut.h"
#include "scattering_lut.h"

namespace imp {
  class atmosphere_system {
  public:
    atmosphere_system(
        atmosphere_system_create_info const &create_info,
        gpu_allocator &allocator);

  private:
    vk::UniqueDescriptorSetLayout lut_descriptor_set_layout_;
    vk::UniquePipelineLayout scattering_pipeline_layout_;
    vk::UniquePipelineLayout optical_depth_pipeline_layout_;
    vk::UniquePipeline scattering_pipeline_;
    vk::UniquePipeline optical_depth_pipeline_;
    vk::UniqueDescriptorPool descriptor_pool_;
    vk::UniqueDescriptorSet scattering_descriptor_set_;
    vk::UniqueDescriptorSet optical_depth_descriptor_set_;
    scattering_lut scattering_lut_;
    optical_depth_lut optical_depth_lut_;

    vk::UniqueDescriptorSetLayout create_lut_descriptor_set_layout(
        atmosphere_system_create_info const &create_info);
    vk::UniquePipelineLayout create_scattering_pipeline_layout(
        atmosphere_system_create_info const &create_info);
    vk::UniquePipelineLayout create_optical_depth_pipeline_layout(
        atmosphere_system_create_info const &create_info);
    vk::UniquePipeline create_scattering_pipeline();
    vk::UniquePipeline create_optical_depth_pipeline();
    scattering_lut create_scattering_lut(
        atmosphere_system_create_info const &create_info,
        gpu_allocator &allocator);
    optical_depth_lut create_optical_depth_lut(
        atmosphere_system_create_info const &create_info,
        gpu_allocator &allocator);
  };
} // namespace imp