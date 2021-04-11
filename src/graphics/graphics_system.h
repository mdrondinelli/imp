#pragma once

#include <filesystem>

#include <entt/entt.hpp>

#include "../core/window.h"
#include "../math/scalar.h"
#include "gpu_frame.h"
#include "gpu_sampler_pool.h"
#include "optical_depth_lut.h"
#include "scattering_lut.h"

namespace imp {
  class graphics_system {
  public:
    graphics_system(window_manager *win, entt::registry *reg);
    ~graphics_system();
    void draw();

  private:
    struct physical_device_info {
      vk::PhysicalDevice handle;
      uint32_t graphics_family;
      uint32_t compute_family;
      uint32_t present_family;
      vk::Format swapchain_image_format;
      vk::ColorSpaceKHR swapchain_image_color_space;
      vk::PresentModeKHR swapchain_present_mode;
    };

    struct logical_device_info {
      vk::UniqueDevice handle;
      vk::Queue graphics_queue;
      vk::Queue compute_queue;
      vk::Queue present_queue;
    };

    window_manager *window_;
    entt::registry *registry_;
    vk::UniqueInstance instance_;
    vk::UniqueSurfaceKHR surface_;
    physical_device_info physical_device_;
    logical_device_info logical_device_;
    vk::UniqueCommandPool compute_command_pool_;
    vk::UniqueRenderPass atmosphere_pass_;
    vk::UniqueDescriptorSetLayout compute_lut_descriptor_set_layout_;
    vk::UniqueDescriptorSetLayout atmosphere_descriptor_set_layout_;
    vk::UniquePipelineLayout scattering_pipeline_layout_;
    vk::UniquePipelineLayout optical_depth_pipeline_layout_;
    vk::UniquePipelineLayout atmosphere_pipeline_layout_;
    vk::UniquePipeline scattering_pipeline_;
    vk::UniquePipeline optical_depth_pipeline_;
    vk::UniquePipeline atmosphere_pipeline_;
    vk::UniqueDescriptorPool descriptor_pool_;
    vk::UniqueDescriptorSet scattering_descriptor_set_;
    vk::UniqueDescriptorSet optical_depth_descriptor_set_;
    vk::UniqueDescriptorSet atmosphere_descriptor_set_;
    gpu_sampler_pool sampler_pool_;
    vk::Sampler lut_sampler_;
    gpu_allocator allocator_;
    scattering_lut scattering_lut_;
    optical_depth_lut optical_depth_lut_;
    std::vector<gpu_frame> frames_;
    size_t frame_;
    vk::UniqueSwapchainKHR swapchain_;
    std::vector<vk::Image> swapchain_images_;
    std::vector<vk::UniqueImageView> swapchain_image_views_;
    std::vector<vk::UniqueFramebuffer> swapchain_framebuffers_;

    vk::UniqueInstance create_instance();
    vk::UniqueSurfaceKHR create_surface();
    physical_device_info select_physical_device();
    logical_device_info create_logical_device();
    vk::UniqueCommandPool create_compute_command_pool();
    vk::UniqueRenderPass create_atmosphere_pass();
    vk::UniqueDescriptorSetLayout create_compute_lut_descriptor_set_layout();
    vk::UniqueDescriptorSetLayout create_atmosphere_descriptor_set_layout();
    vk::UniqueShaderModule
    create_shader_module(std::filesystem::path const &path);
    vk::UniquePipelineLayout create_scattering_pipeline_layout();
    vk::UniquePipelineLayout create_optical_depth_pipeline_layout();
    vk::UniquePipelineLayout create_atmosphere_pipeline_layout();
    vk::UniquePipeline create_scattering_pipeline();
    vk::UniquePipeline create_optical_depth_pipeline();
    vk::UniquePipeline create_atmosphere_pipeline();
    vk::UniqueDescriptorPool create_descriptor_pool();
    vk::UniqueDescriptorSet allocate_compute_lut_descriptor_set();
    vk::UniqueDescriptorSet allocate_atmosphere_descriptor_set();
    vk::Sampler create_lut_sampler();
    scattering_lut create_scattering_lut();
    optical_depth_lut create_optical_depth_lut();
    std::vector<gpu_frame> create_frames();

    void update_scattering_descriptor_set();
    void update_optical_depth_descriptor_set();
    void update_atmosphere_descriptor_set();
    void update_scattering_lut();
    void update_optical_depth_lut();
    void create_swapchain();
  };
} // namespace imp