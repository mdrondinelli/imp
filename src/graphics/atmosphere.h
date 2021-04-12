#pragma once

#include "../core/gpu_context.h"
#include "optical_depth_lut.h"
#include "scattering_lut.h"

namespace imp {
  class atmosphere {
  public:
    atmosphere(
        gpu_context &context,
        vector4f const &scattering_coefficients =
            make_vector(3.8e-6f, 13.5e-6f, 33.1e-6f, 21e-6f),
        float planet_radius = 6360e3f,
        float atmosphere_radius = 6420e3f);

    scattering_lut const &scattering() const noexcept;
    optical_depth_lut const &optical_depth() const noexcept;

  private:
    gpu_context *context_;
    vector4f scattering_coefficients_;
    float planet_radius_;
    float atmosphere_radius_;
    vk::UniqueCommandPool command_pool_;
    vk::UniqueDescriptorSetLayout lut_descriptor_set_layout_;
    vk::UniquePipelineLayout scattering_pipeline_layout_;
    vk::UniquePipelineLayout optical_depth_pipeline_layout_;
    vk::UniquePipeline scattering_pipeline_;
    vk::UniquePipeline optical_depth_pipeline_;
    vk::UniqueDescriptorPool descriptor_pool_;
    std::vector<vk::DescriptorSet> descriptor_sets_;
    scattering_lut scattering_;
    optical_depth_lut optical_depth_;

    vk::UniqueCommandPool create_command_pool();
    vk::UniqueDescriptorSetLayout create_lut_descriptor_set_layout();
    vk::UniquePipelineLayout create_scattering_pipeline_layout();
    vk::UniquePipelineLayout create_optical_depth_pipeline_layout();
    vk::UniquePipeline create_scattering_pipeline();
    vk::UniquePipeline create_optical_depth_pipeline();
    vk::UniqueDescriptorPool create_descriptor_pool();
    std::vector<vk::DescriptorSet> allocate_descriptor_sets();

    void update_descriptor_sets();
    void update_scattering_lut();
    void update_optical_depth_lut();
  };
} // namespace imp