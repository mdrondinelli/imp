#pragma once

#include <filesystem>

#include "../core/window.h"
#include "atmosphere.h"
#include "gpu_frame.h"

namespace imp {
  class renderer {
  public:
    renderer(gpu_context &context, window &window);
    ~renderer();

    void render();

  private:
    gpu_context *context_;
    window *window_;
    atmosphere atmosphere_;
    vk::UniqueRenderPass atmosphere_pass_;
    vk::UniqueDescriptorSetLayout atmosphere_descriptor_set_layout_;
    vk::UniquePipelineLayout atmosphere_pipeline_layout_;
    vk::UniquePipeline atmosphere_pipeline_;
    vk::UniqueDescriptorPool descriptor_pool_;
    vk::UniqueDescriptorSet atmosphere_descriptor_set_;
    vk::Sampler lut_sampler_;
    std::vector<gpu_frame> frames_;
    size_t frame_;

    vk::UniqueRenderPass create_atmosphere_pass();
    vk::UniqueDescriptorSetLayout create_atmosphere_descriptor_set_layout();
    vk::UniqueShaderModule
    create_shader_module(std::filesystem::path const &path);
    vk::UniquePipelineLayout create_atmosphere_pipeline_layout();
    vk::UniquePipeline create_atmosphere_pipeline();
    vk::UniqueDescriptorPool create_descriptor_pool();
    vk::UniqueDescriptorSet allocate_atmosphere_descriptor_set();
    vk::Sampler create_lut_sampler();
    std::vector<gpu_frame> create_frames();

    void update_atmosphere_descriptor_set();
  };
} // namespace imp