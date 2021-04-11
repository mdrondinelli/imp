#include "graphics_system.h"

#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "../math/matrix.h"

namespace imp {
  graphics_system::graphics_system(window_manager *win, entt::registry *reg):
      window_{win},
      registry_{reg},
      instance_{create_instance()},
      surface_{create_surface()},
      physical_device_{select_physical_device()},
      logical_device_{create_logical_device()},
      compute_command_pool_{create_compute_command_pool()},
      atmosphere_pass_{create_atmosphere_pass()},
      compute_lut_descriptor_set_layout_{
          create_compute_lut_descriptor_set_layout()},
      atmosphere_descriptor_set_layout_{
          create_atmosphere_descriptor_set_layout()},
      scattering_pipeline_layout_{create_scattering_pipeline_layout()},
      optical_depth_pipeline_layout_{create_optical_depth_pipeline_layout()},
      atmosphere_pipeline_layout_{create_atmosphere_pipeline_layout()},
      scattering_pipeline_{create_scattering_pipeline()},
      optical_depth_pipeline_{create_optical_depth_pipeline()},
      atmosphere_pipeline_{create_atmosphere_pipeline()},
      descriptor_pool_{create_descriptor_pool()},
      scattering_descriptor_set_{allocate_compute_lut_descriptor_set()},
      optical_depth_descriptor_set_{allocate_compute_lut_descriptor_set()},
      atmosphere_descriptor_set_{allocate_atmosphere_descriptor_set()},
      sampler_pool_{*logical_device_.handle},
      lut_sampler_{create_lut_sampler()},
      allocator_{
          {*instance_, physical_device_.handle, *logical_device_.handle}},
      scattering_lut_{create_scattering_lut()},
      optical_depth_lut_{create_optical_depth_lut()},
      frames_{create_frames()},
      frame_{0} {
    update_scattering_descriptor_set();
    update_optical_depth_descriptor_set();
    update_atmosphere_descriptor_set();
    update_scattering_lut();
    update_optical_depth_lut();
    create_swapchain();
  }

  graphics_system::~graphics_system() {
    logical_device_.handle->waitIdle();
  }

  vk::UniqueInstance graphics_system::create_instance() {
    auto app_info = vk::ApplicationInfo{};
    app_info.pApplicationName = "imp";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "imp";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;
    auto layers = std::array{"VK_LAYER_KHRONOS_validation"};
    auto extension_count = uint32_t{};
    auto extensions = glfwGetRequiredInstanceExtensions(&extension_count);
    auto instance_info = vk::InstanceCreateInfo{};
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instance_info.ppEnabledLayerNames = layers.data();
    instance_info.enabledExtensionCount = extension_count;
    instance_info.ppEnabledExtensionNames = extensions;
    return vk::createInstanceUnique(instance_info);
  }

  vk::UniqueSurfaceKHR graphics_system::create_surface() {
    auto surface = VkSurfaceKHR{};
    if (glfwCreateWindowSurface(
            instance_.get(), window_->get(), nullptr, &surface)) {
      throw std::runtime_error{"Failed to create vulkan surface."};
    }
    return vk::UniqueSurfaceKHR{surface, instance_.get()};
  }

  graphics_system::physical_device_info
  graphics_system::select_physical_device() {
    auto physical_devices = instance_->enumeratePhysicalDevices();
    if (physical_devices.empty()) {
      throw std::runtime_error{"Failed to find vulkan physical device."};
    }
    auto physical_device = physical_device_info{};
    for (auto curr_physical_device : physical_devices) {
      auto properties = curr_physical_device.getProperties();
      if (properties.apiVersion < VK_API_VERSION_1_1) {
        continue;
      }
      auto available_extensions =
          curr_physical_device.enumerateDeviceExtensionProperties();
      auto required_extensions =
          std::unordered_set<std::string>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
      for (auto &available_extension : available_extensions) {
        required_extensions.erase(available_extension.extensionName);
      }
      if (!required_extensions.empty()) {
        continue;
      }
      auto features = curr_physical_device.getFeatures();
      if (!features.shaderSampledImageArrayDynamicIndexing) {
        continue;
      }
      auto queue_families = curr_physical_device.getQueueFamilyProperties();
      auto graphics_family = std::optional<uint32_t>{};
      auto compute_family = std::optional<uint32_t>{};
      auto present_family = std::optional<uint32_t>{};
      for (auto i = uint32_t{0};
           i < queue_families.size() && !graphics_family && !compute_family;
           ++i) {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
          graphics_family = i;
        }
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eCompute) {
          compute_family = i;
        }
      }
      for (auto i = uint32_t{0}; i < queue_families.size() && !present_family;
           ++i) {
        if (curr_physical_device.getSurfaceSupportKHR(i, *surface_)) {
          present_family = i;
        }
      }
      if (!graphics_family || !compute_family || !present_family) {
        continue;
      }
      if (physical_device.handle) {
        auto best_type = physical_device.handle.getProperties().deviceType;
        auto curr_type = curr_physical_device.getProperties().deviceType;
        auto priority = [](vk::PhysicalDeviceType type) {
          switch (type) {
          default:
          case vk::PhysicalDeviceType::eOther:
            return 0;
          case vk::PhysicalDeviceType::eCpu:
            return 1;
          case vk::PhysicalDeviceType::eVirtualGpu:
            return 2;
          case vk::PhysicalDeviceType::eIntegratedGpu:
            return 3;
          case vk::PhysicalDeviceType::eDiscreteGpu:
            return 4;
          }
        };
        if (priority(best_type) < priority(curr_type)) {
          physical_device.handle = curr_physical_device;
          physical_device.graphics_family = *graphics_family;
          physical_device.compute_family = *compute_family;
          physical_device.present_family = *present_family;
        }
      } else {
        physical_device.handle = curr_physical_device;
        physical_device.graphics_family = *graphics_family;
        physical_device.compute_family = *compute_family;
        physical_device.present_family = *present_family;
      }
    }
    if (!physical_device.handle) {
      throw std::runtime_error{"Failed to select vulkan physical device."};
    }
    auto formats = physical_device.handle.getSurfaceFormatsKHR(*surface_);
    auto format = formats[0];
    for (auto i = size_t{1}; i < formats.size(); ++i) {
      if (formats[i].format == vk::Format::eB8G8R8A8Srgb &&
          formats[i].colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        format = formats[i];
      }
    }
    auto present_modes =
        physical_device.handle.getSurfacePresentModesKHR(*surface_);
    auto present_mode = present_modes[0];
    for (auto i = size_t{1}; i < present_modes.size(); ++i) {
      auto priority = [](vk::PresentModeKHR mode) {
        switch (mode) {
        default:
        case vk::PresentModeKHR::eFifo:
          return 0;
        case vk::PresentModeKHR::eFifoRelaxed:
          return 1;
        case vk::PresentModeKHR::eImmediate:
          return 2;
        case vk::PresentModeKHR::eMailbox:
          return 3;
        }
      };
      if (priority(present_mode) < priority(present_modes[i]))
        present_mode = present_modes[i];
    }
    physical_device.swapchain_image_format = format.format;
    physical_device.swapchain_image_color_space = format.colorSpace;
    physical_device.swapchain_present_mode = present_mode;
    return physical_device;
  }

  graphics_system::logical_device_info
  graphics_system::create_logical_device() {
    auto queue_infos = std::vector<vk::DeviceQueueCreateInfo>{};
    auto queue_priority = 1.0f;
    for (auto queue_family : std::unordered_set{
             physical_device_.graphics_family,
             physical_device_.compute_family,
             physical_device_.present_family}) {
      auto queue_info = vk::DeviceQueueCreateInfo{};
      queue_info.queueFamilyIndex = queue_family;
      queue_info.queueCount = 1;
      queue_info.pQueuePriorities = &queue_priority;
      queue_infos.emplace_back(queue_info);
    }
    auto extensions = std::array{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    auto device_info = vk::DeviceCreateInfo{};
    device_info.queueCreateInfoCount =
        static_cast<uint32_t>(queue_infos.size());
    device_info.pQueueCreateInfos = queue_infos.data();
    device_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    device_info.ppEnabledExtensionNames = extensions.data();
    auto logical_device = logical_device_info{};
    logical_device.handle =
        physical_device_.handle.createDeviceUnique(device_info);
    logical_device.graphics_queue =
        logical_device.handle->getQueue(physical_device_.graphics_family, 0);
    logical_device.compute_queue =
        logical_device.handle->getQueue(physical_device_.compute_family, 0);
    logical_device.present_queue =
        logical_device.handle->getQueue(physical_device_.present_family, 0);
    return logical_device;
  }

  vk::UniqueCommandPool graphics_system::create_compute_command_pool() {
    auto info = vk::CommandPoolCreateInfo{};
    info.flags = vk::CommandPoolCreateFlagBits::eTransient;
    info.queueFamilyIndex = physical_device_.compute_family;
    return logical_device_.handle->createCommandPoolUnique(info);
  }

  vk::UniqueRenderPass graphics_system::create_atmosphere_pass() {
    auto attachments = std::array<vk::AttachmentDescription, 1>{};
    auto &color_attachment = attachments[0];
    color_attachment.format = physical_device_.swapchain_image_format;
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
    color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    auto subpass_color_attachments = std::array<vk::AttachmentReference, 1>{};
    auto &subpass_color_attachment = subpass_color_attachments[0];
    subpass_color_attachment.attachment = 0;
    subpass_color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpasses = std::array<vk::SubpassDescription, 1>{};
    auto &subpass = subpasses[0];
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount =
        static_cast<uint32_t>(subpass_color_attachments.size());
    subpass.pColorAttachments = subpass_color_attachments.data();
    auto info = vk::RenderPassCreateInfo{};
    info.attachmentCount = static_cast<uint32_t>(attachments.size());
    info.pAttachments = attachments.data();
    info.subpassCount = static_cast<uint32_t>(subpasses.size());
    info.pSubpasses = subpasses.data();
    return logical_device_.handle->createRenderPassUnique(info);
  }

  vk::UniqueDescriptorSetLayout
  graphics_system::create_compute_lut_descriptor_set_layout() {
    auto lut_binding = vk::DescriptorSetLayoutBinding{};
    lut_binding.binding = 0;
    lut_binding.descriptorType = vk::DescriptorType::eStorageImage;
    lut_binding.descriptorCount = 1;
    lut_binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
    auto bindings = std::array{lut_binding};
    auto info = vk::DescriptorSetLayoutCreateInfo{};
    info.bindingCount = static_cast<uint32_t>(bindings.size());
    info.pBindings = bindings.data();
    return logical_device_.handle->createDescriptorSetLayoutUnique(info);
  }

  vk::UniqueDescriptorSetLayout
  graphics_system::create_atmosphere_descriptor_set_layout() {
    auto scattering_binding = vk::DescriptorSetLayoutBinding{};
    scattering_binding.binding = 0;
    scattering_binding.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    scattering_binding.descriptorCount = 1;
    scattering_binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto optical_depth_binding = vk::DescriptorSetLayoutBinding{};
    optical_depth_binding.binding = 1;
    optical_depth_binding.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    optical_depth_binding.descriptorCount = 1;
    optical_depth_binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto bindings = std::array{scattering_binding, optical_depth_binding};
    auto info = vk::DescriptorSetLayoutCreateInfo{};
    info.bindingCount = static_cast<uint32_t>(bindings.size());
    info.pBindings = bindings.data();
    return logical_device_.handle->createDescriptorSetLayoutUnique(info);
  }

  vk::UniqueShaderModule
  graphics_system::create_shader_module(std::filesystem::path const &path) {
    auto code = std::vector<char>{};
    auto in = std::ifstream{};
    in.exceptions(std::ios::badbit | std::ios::failbit);
    in.open(path, std::ios::binary);
    in.seekg(0, std::ios::end);
    auto size = static_cast<size_t>(in.tellg());
    if (size % sizeof(uint32_t) != 0) {
      throw std::runtime_error{"failed to create shader module"};
    }
    code.resize(size);
    in.seekg(0, std::ios::beg);
    in.read(code.data(), code.size());
    auto module_info = vk::ShaderModuleCreateInfo{};
    module_info.codeSize = static_cast<uint32_t>(code.size());
    module_info.pCode = reinterpret_cast<uint32_t *>(code.data());
    return logical_device_.handle->createShaderModuleUnique(module_info);
  }

  vk::UniquePipelineLayout
  graphics_system::create_scattering_pipeline_layout() {
    auto set_layouts = std::array{*compute_lut_descriptor_set_layout_};
    auto push_constant_range = vk::PushConstantRange{};
    push_constant_range.stageFlags = vk::ShaderStageFlagBits::eCompute;
    push_constant_range.offset = 0;
    push_constant_range.size = 28;
    auto info = vk::PipelineLayoutCreateInfo{};
    info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &push_constant_range;
    return logical_device_.handle->createPipelineLayoutUnique(info);
  }

  vk::UniquePipelineLayout
  graphics_system::create_optical_depth_pipeline_layout() {
    auto set_layouts = std::array{*compute_lut_descriptor_set_layout_};
    auto push_constant_range = vk::PushConstantRange{};
    push_constant_range.stageFlags = vk::ShaderStageFlagBits::eCompute;
    push_constant_range.offset = 0;
    push_constant_range.size = 28;
    auto info = vk::PipelineLayoutCreateInfo{};
    info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &push_constant_range;
    return logical_device_.handle->createPipelineLayoutUnique(info);
  }

  vk::UniquePipelineLayout
  graphics_system::create_atmosphere_pipeline_layout() {
    auto set_layouts = std::array{*atmosphere_descriptor_set_layout_};
    auto push_constant_range = vk::PushConstantRange{};
    push_constant_range.stageFlags =
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    push_constant_range.offset = 0;
    push_constant_range.size = 124;
    auto info = vk::PipelineLayoutCreateInfo{};
    info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    info.pSetLayouts = set_layouts.data();
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &push_constant_range;
    return logical_device_.handle->createPipelineLayoutUnique(info);
  }

  vk::UniquePipeline graphics_system::create_scattering_pipeline() {
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
  }

  vk::UniquePipeline graphics_system::create_optical_depth_pipeline() {
    auto module = create_shader_module("./res/optical_depth.spv");
    auto pipeline_info = vk::ComputePipelineCreateInfo{};
    pipeline_info.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipeline_info.stage.module = *module;
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = *optical_depth_pipeline_layout_;
    pipeline_info.basePipelineIndex = -1;
    return logical_device_.handle
        ->createComputePipelineUnique({}, pipeline_info)
        .value;
  }

  vk::UniquePipeline graphics_system::create_atmosphere_pipeline() {
    auto vert_module = create_shader_module("./res/atmosphere-vert.spv");
    auto vert_stage = vk::PipelineShaderStageCreateInfo{};
    vert_stage.stage = vk::ShaderStageFlagBits::eVertex;
    vert_stage.module = *vert_module;
    vert_stage.pName = "main";
    auto frag_module = create_shader_module("./res/atmosphere-frag.spv");
    auto frag_stage = vk::PipelineShaderStageCreateInfo{};
    frag_stage.stage = vk::ShaderStageFlagBits::eFragment;
    frag_stage.module = *frag_module;
    frag_stage.pName = "main";
    auto stages = std::array{vert_stage, frag_stage};
    auto vertex_input_state = vk::PipelineVertexInputStateCreateInfo{};
    auto input_assembly_state = vk::PipelineInputAssemblyStateCreateInfo{};
    input_assembly_state.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewport_state = vk::PipelineViewportStateCreateInfo{};
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    auto rasterization_state = vk::PipelineRasterizationStateCreateInfo{};
    rasterization_state.polygonMode = vk::PolygonMode::eFill;
    rasterization_state.cullMode = vk::CullModeFlagBits::eNone;
    rasterization_state.frontFace = vk::FrontFace::eCounterClockwise;
    rasterization_state.lineWidth = 1.0f;
    auto multisample_state = vk::PipelineMultisampleStateCreateInfo{};
    multisample_state.rasterizationSamples = vk::SampleCountFlagBits::e1;
    // auto depth_stencil_state = vk::PipelineDepthStencilStateCreateInfo{};
    // depth_stencil_state.depthTestEnable = false;
    // depth_stencil_state.depthCompareOp = vk::CompareOp::eEqual;
    // depth_stencil_state.minDepthBounds = 0.0f;
    // depth_stencil_state.maxDepthBounds = 1.0f;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto color_blend_state = vk::PipelineColorBlendStateCreateInfo{};
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &attachment;
    auto dynamic_states =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamic_state = vk::PipelineDynamicStateCreateInfo{};
    dynamic_state.dynamicStateCount =
        static_cast<uint32_t>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();
    auto info = vk::GraphicsPipelineCreateInfo{};
    info.stageCount = static_cast<uint32_t>(stages.size());
    info.pStages = stages.data();
    info.pVertexInputState = &vertex_input_state;
    info.pInputAssemblyState = &input_assembly_state;
    info.pViewportState = &viewport_state;
    info.pRasterizationState = &rasterization_state;
    info.pMultisampleState = &multisample_state;
    // info.pDepthStencilState = &depth_stencil_state;
    info.pColorBlendState = &color_blend_state;
    info.pDynamicState = &dynamic_state;
    info.layout = *atmosphere_pipeline_layout_;
    info.renderPass = *atmosphere_pass_;
    info.subpass = 0;
    info.basePipelineHandle = vk::Pipeline{};
    info.basePipelineIndex = -1;
    return logical_device_.handle->createGraphicsPipelineUnique({}, info).value;
  }

  vk::UniqueDescriptorPool graphics_system::create_descriptor_pool() {
    auto pool_sizes = std::vector<vk::DescriptorPoolSize>{
        // scattering
        {vk::DescriptorType::eStorageImage, 1},
        // optical_depth
        {vk::DescriptorType::eStorageImage, 1},
        // atmosphere
        {vk::DescriptorType::eCombinedImageSampler, 2}};
    auto info = vk::DescriptorPoolCreateInfo{};
    info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    info.maxSets = 3;
    info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    info.pPoolSizes = pool_sizes.data();
    return logical_device_.handle->createDescriptorPoolUnique(info);
  }

  vk::UniqueDescriptorSet
  graphics_system::allocate_compute_lut_descriptor_set() {
    auto info = vk::DescriptorSetAllocateInfo{};
    info.descriptorPool = *descriptor_pool_;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &*compute_lut_descriptor_set_layout_;
    auto sets = logical_device_.handle->allocateDescriptorSetsUnique(info);
    return std::move(sets[0]);
  }

  vk::UniqueDescriptorSet
  graphics_system::allocate_atmosphere_descriptor_set() {
    auto info = vk::DescriptorSetAllocateInfo{};
    info.descriptorPool = *descriptor_pool_;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &*atmosphere_descriptor_set_layout_;
    auto sets = logical_device_.handle->allocateDescriptorSetsUnique(info);
    return std::move(sets[0]);
  }

  vk::Sampler graphics_system::create_lut_sampler() {
    auto info = gpu_sampler_create_info{};
    info.mag_filter = vk::Filter::eLinear;
    info.min_filter = vk::Filter::eLinear;
    info.mipmap_mode = vk::SamplerMipmapMode::eNearest;
    info.address_mode_u = vk::SamplerAddressMode::eClampToEdge;
    info.address_mode_v = vk::SamplerAddressMode::eClampToEdge;
    info.address_mode_w = vk::SamplerAddressMode::eClampToEdge;
    return sampler_pool_.create(info);
  }

  scattering_lut graphics_system::create_scattering_lut() {
    auto info = scattering_lut_create_info{};
    info.size[0] = 32;
    info.size[1] = 512;
    info.size[2] = 256;
    info.graphics_family = physical_device_.graphics_family;
    info.compute_family = physical_device_.compute_family;
    return scattering_lut{info, allocator_};
  }

  optical_depth_lut graphics_system::create_optical_depth_lut() {
    auto info = optical_depth_lut_create_info{};
    info.size[0] = 512;
    info.size[1] = 512;
    info.graphics_family = physical_device_.graphics_family;
    info.compute_family = physical_device_.compute_family;
    return optical_depth_lut{info, allocator_};
  }

  std::vector<gpu_frame> graphics_system::create_frames() {
    auto frames = std::vector<gpu_frame>();
    frames.reserve(3);
    for (auto i = size_t{}; i < frames.capacity(); ++i) {
      frames.emplace_back(
          *logical_device_.handle, physical_device_.graphics_family);
    }
    return frames;
  }

  void graphics_system::update_scattering_descriptor_set() {
    auto image_info = vk::DescriptorImageInfo{};
    image_info.imageView = scattering_lut_.image_view();
    image_info.imageLayout = vk::ImageLayout::eGeneral;
    auto write = vk::WriteDescriptorSet{};
    write.dstSet = *scattering_descriptor_set_;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eStorageImage;
    write.pImageInfo = &image_info;
    logical_device_.handle->updateDescriptorSets(write, {});
  }

  void graphics_system::update_optical_depth_descriptor_set() {
    auto image_info = vk::DescriptorImageInfo{};
    image_info.imageView = optical_depth_lut_.image_view();
    image_info.imageLayout = vk::ImageLayout::eGeneral;
    auto write = vk::WriteDescriptorSet{};
    write.dstSet = *optical_depth_descriptor_set_;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eStorageImage;
    write.pImageInfo = &image_info;
    logical_device_.handle->updateDescriptorSets(write, {});
  }

  void graphics_system::update_atmosphere_descriptor_set() {
    auto scattering_lut_info = vk::DescriptorImageInfo{};
    scattering_lut_info.sampler = lut_sampler_;
    scattering_lut_info.imageView = scattering_lut_.image_view();
    scattering_lut_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto scattering_lut_write = vk::WriteDescriptorSet{};
    scattering_lut_write.dstSet = *atmosphere_descriptor_set_;
    scattering_lut_write.dstBinding = 0;
    scattering_lut_write.dstArrayElement = 0;
    scattering_lut_write.descriptorCount = 1;
    scattering_lut_write.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    scattering_lut_write.pImageInfo = &scattering_lut_info;
    auto optical_depth_lut_info = vk::DescriptorImageInfo{};
    optical_depth_lut_info.sampler = lut_sampler_;
    optical_depth_lut_info.imageView = optical_depth_lut_.image_view();
    optical_depth_lut_info.imageLayout =
        vk::ImageLayout::eShaderReadOnlyOptimal;
    auto optical_depth_lut_write = vk::WriteDescriptorSet{};
    optical_depth_lut_write.dstSet = *atmosphere_descriptor_set_;
    optical_depth_lut_write.dstBinding = 1;
    optical_depth_lut_write.dstArrayElement = 0;
    optical_depth_lut_write.descriptorCount = 1;
    optical_depth_lut_write.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    optical_depth_lut_write.pImageInfo = &optical_depth_lut_info;
    auto writes = std::array{scattering_lut_write, optical_depth_lut_write};
    logical_device_.handle->updateDescriptorSets(writes, {});
  }

  void graphics_system::update_scattering_lut() {
    auto alloc_info = vk::CommandBufferAllocateInfo{};
    alloc_info.commandPool = *compute_command_pool_;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandBufferCount = 1;
    auto command_buffer = std::move(
        logical_device_.handle->allocateCommandBuffersUnique(alloc_info)[0]);
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
    layout_barrier.image = scattering_lut_.image();
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
        *scattering_descriptor_set_,
        {});
    auto scattering_coefficients =
        make_vector(3.8e-6f, 13.5e-6f, 33.1e-6f, 21e-6f);
    auto planet_radius = 6360e3f;
    auto atmosphere_radius = 6420e3f;
    auto integration_steps = 64;
    auto push_constants = std::array<char, 28>{};
    std::memcpy(&push_constants[0], &scattering_coefficients, 16);
    std::memcpy(&push_constants[16], &planet_radius, 4);
    std::memcpy(&push_constants[20], &atmosphere_radius, 4);
    std::memcpy(&push_constants[24], &integration_steps, 4);
    command_buffer->pushConstants(
        *scattering_pipeline_layout_,
        vk::ShaderStageFlagBits::eCompute,
        0,
        push_constants.size(),
        push_constants.data());
    auto group_counts = scattering_lut_.size() / 4;
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
    logical_device_.compute_queue.submit(submit_info);
    logical_device_.compute_queue.waitIdle();
  }

  void graphics_system::update_optical_depth_lut() {
    auto alloc_info = vk::CommandBufferAllocateInfo{};
    alloc_info.commandPool = *compute_command_pool_;
    alloc_info.level = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandBufferCount = 1;
    auto command_buffer = std::move(
        logical_device_.handle->allocateCommandBuffersUnique(alloc_info)[0]);
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
    layout_barrier.image = optical_depth_lut_.image();
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
        *optical_depth_descriptor_set_,
        {});
    auto scattering_coefficients =
        make_vector(3.8e-6f, 13.5e-6f, 33.1e-6f, 21e-6f);
    auto planet_radius = 6360e3f;
    auto atmosphere_radius = 6420e3f;
    auto integration_steps = 64;
    auto push_constants = std::array<char, 28>{};
    std::memcpy(&push_constants[0], &scattering_coefficients, 16);
    std::memcpy(&push_constants[16], &planet_radius, 4);
    std::memcpy(&push_constants[20], &atmosphere_radius, 4);
    std::memcpy(&push_constants[24], &integration_steps, 4);
    command_buffer->pushConstants(
        *optical_depth_pipeline_layout_,
        vk::ShaderStageFlagBits::eCompute,
        0,
        push_constants.size(),
        push_constants.data());
    auto group_counts = optical_depth_lut_.size() / 8;
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
    logical_device_.compute_queue.submit(submit_info);
    logical_device_.compute_queue.waitIdle();
  }

  void graphics_system::create_swapchain() {
    logical_device_.handle->waitIdle();
    swapchain_framebuffers_.clear();
    swapchain_image_views_.clear();
    swapchain_images_.clear();
    swapchain_.reset();
    auto info = vk::SwapchainCreateInfoKHR{};
    info.surface = *surface_;
    auto capabilities =
        physical_device_.handle.getSurfaceCapabilitiesKHR(*surface_);
    info.minImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount != 0 &&
        info.minImageCount > capabilities.maxImageCount) {
      info.minImageCount = capabilities.maxImageCount;
    }
    info.imageFormat = physical_device_.swapchain_image_format;
    info.imageColorSpace = physical_device_.swapchain_image_color_space;
    if (capabilities.currentExtent.width ==
        std::numeric_limits<uint32_t>::max()) {
      info.imageExtent.width = clamp(
          static_cast<uint32_t>(window_->framebuffer_width()),
          capabilities.minImageExtent.width,
          capabilities.maxImageExtent.width);
      info.imageExtent.height = clamp(
          static_cast<uint32_t>(window_->framebuffer_height()),
          capabilities.minImageExtent.height,
          capabilities.maxImageExtent.height);
    } else {
      info.imageExtent = capabilities.currentExtent;
    }
    info.imageArrayLayers = 1;
    info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    auto queue_family_indices = std::array{
        physical_device_.graphics_family, physical_device_.present_family};
    if (queue_family_indices[0] == queue_family_indices[1]) {
      info.imageSharingMode = vk::SharingMode::eExclusive;
    } else {
      info.imageSharingMode = vk::SharingMode::eConcurrent;
      info.queueFamilyIndexCount =
          static_cast<uint32_t>(queue_family_indices.size());
      info.pQueueFamilyIndices = queue_family_indices.data();
    }
    info.preTransform = capabilities.currentTransform;
    info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    info.presentMode = physical_device_.swapchain_present_mode;
    info.clipped = true;
    swapchain_ = logical_device_.handle->createSwapchainKHRUnique(info);
    swapchain_images_ =
        logical_device_.handle->getSwapchainImagesKHR(*swapchain_);
    for (auto image : swapchain_images_) {
      auto view_info = vk::ImageViewCreateInfo{};
      view_info.image = image;
      view_info.viewType = vk::ImageViewType::e2D;
      view_info.format = physical_device_.swapchain_image_format;
      view_info.components = vk::ComponentMapping{};
      view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      view_info.subresourceRange.baseMipLevel = 0;
      view_info.subresourceRange.levelCount = 1;
      view_info.subresourceRange.baseArrayLayer = 0;
      view_info.subresourceRange.layerCount = 1;
      swapchain_image_views_.emplace_back(
          logical_device_.handle->createImageViewUnique(view_info));
    }
    for (auto &image_view : swapchain_image_views_) {
      auto framebuffer_info = vk::FramebufferCreateInfo{};
      framebuffer_info.renderPass = *atmosphere_pass_;
      auto attachments = std::array{*image_view};
      framebuffer_info.attachmentCount =
          static_cast<uint32_t>(attachments.size());
      framebuffer_info.pAttachments = attachments.data();
      framebuffer_info.width = info.imageExtent.width;
      framebuffer_info.height = info.imageExtent.height;
      framebuffer_info.layers = 1;
      swapchain_framebuffers_.emplace_back(
          logical_device_.handle->createFramebufferUnique(framebuffer_info));
    }
  }

  void graphics_system::draw() {
    try {
      auto &frame = frames_[frame_ % frames_.size()];
      auto image_acquisition_semaphore = frame.image_acquisition_semaphore();
      auto queue_submission_semaphore = frame.queue_submission_semaphore();
      auto queue_submission_fence = frame.queue_submission_fence();
      auto command_pool = frame.command_pool();
      auto command_buffer = frame.command_buffer();
      auto image_index = logical_device_.handle
                             ->acquireNextImageKHR(
                                 *swapchain_,
                                 std::numeric_limits<uint64_t>::max(),
                                 image_acquisition_semaphore)
                             .value;
      auto framebuffer = *swapchain_framebuffers_[image_index];
      auto buffer_info = vk::CommandBufferBeginInfo{};
      buffer_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
      auto clear_values = std::array<vk::ClearValue, 1>{};
      clear_values[0].color.float32 = std::array{0.0f, 0.0f, 0.0f, 0.0f};
      auto pass_info = vk::RenderPassBeginInfo{};
      pass_info.renderPass = *atmosphere_pass_;
      pass_info.framebuffer = framebuffer;
      pass_info.renderArea.offset.x = 0;
      pass_info.renderArea.offset.y = 0;
      pass_info.renderArea.extent.width = window_->framebuffer_width();
      pass_info.renderArea.extent.height = window_->framebuffer_height();
      pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
      pass_info.pClearValues = clear_values.data();
      logical_device_.handle->waitForFences(
          queue_submission_fence, true, std::numeric_limits<uint64_t>::max());
      logical_device_.handle->resetFences(queue_submission_fence);
      logical_device_.handle->resetCommandPool(command_pool);
      command_buffer.begin(buffer_info);
      command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);
      command_buffer.bindPipeline(
          vk::PipelineBindPoint::eGraphics, *atmosphere_pipeline_);
      auto viewport = vk::Viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = window_->framebuffer_width();
      viewport.height = window_->framebuffer_height();
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      command_buffer.setViewport(0, viewport);
      auto scissor = vk::Rect2D{};
      scissor.offset.x = 0;
      scissor.offset.y = 0;
      scissor.extent.width = viewport.width;
      scissor.extent.height = viewport.height;
      command_buffer.setScissor(0, scissor);
      command_buffer.bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics,
          *atmosphere_pipeline_layout_,
          0,
          *atmosphere_descriptor_set_,
          {});
      auto frustum_corners = std::array{
          make_vector(-1.0f, 9.0f / 16.0f, -1.0f, 0.0f),
          make_vector(1.0f, 9.0f / 16.0f, -1.0f, 0.0f),
          make_vector(-1.0f, -9.0f / 16.0f, -1.0f, 0.0f),
          make_vector(1.0f, -9.0f / 16.0f, -1.0f, 0.0f)};
      auto eye_position = make_vector(0.0f, 883.0f, 0.0f);
      auto eye_orientation = rotation_matrix4x4(
          rotation_quaternion(0.0f, make_vector(1.0f, 0.0f, 0.0f)));
      for (auto &corner : frustum_corners) {
        corner = eye_orientation * corner;
        corner += concatenate(eye_position, 0.0f);
      }
      auto sun_radiance = make_vector(20.0f, 20.0f, 20.0f);
      auto delta = frame_ / 120000.0f + 1.4f * 3.141592f;
      auto sun_direction =
          normalize(make_vector(0.0f, std::cosf(delta), std::sinf(delta)));
      auto g = 0.76f;
      auto planet_radius = 6360e3f;
      auto atmosphere_radius = 6420e3f;
      auto push_constants = std::array<char, 120>{};
      std::memcpy(push_constants.data() + 0, &frustum_corners, 64);
      std::memcpy(push_constants.data() + 64, &eye_position, 12);
      std::memcpy(push_constants.data() + 80, &sun_radiance, 12);
      std::memcpy(push_constants.data() + 96, &sun_direction, 12);
      std::memcpy(push_constants.data() + 108, &g, 4);
      std::memcpy(push_constants.data() + 112, &planet_radius, 4);
      std::memcpy(push_constants.data() + 116, &atmosphere_radius, 4);
      command_buffer.pushConstants(
          *atmosphere_pipeline_layout_,
          vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
          0,
          push_constants.size(),
          push_constants.data());
      command_buffer.draw(3, 1, 0, 0);
      command_buffer.endRenderPass();
      command_buffer.end();
      auto submit_wait_semaphores = std::array{image_acquisition_semaphore};
      auto submit_wait_stages = std::array<vk::PipelineStageFlags, 1>{
          vk::PipelineStageFlagBits::eColorAttachmentOutput};
      auto submit_command_buffers = std::array{command_buffer};
      auto submit_signal_semaphores = std::array{queue_submission_semaphore};
      auto submit_info = vk::SubmitInfo{};
      submit_info.waitSemaphoreCount =
          static_cast<uint32_t>(submit_wait_semaphores.size());
      submit_info.pWaitSemaphores = submit_wait_semaphores.data();
      submit_info.pWaitDstStageMask = submit_wait_stages.data();
      submit_info.commandBufferCount =
          static_cast<uint32_t>(submit_command_buffers.size());
      submit_info.pCommandBuffers = submit_command_buffers.data();
      submit_info.signalSemaphoreCount =
          static_cast<uint32_t>(submit_signal_semaphores.size());
      submit_info.pSignalSemaphores = submit_signal_semaphores.data();
      logical_device_.graphics_queue.submit(
          submit_info, queue_submission_fence);
      auto present_info = vk::PresentInfoKHR{};
      present_info.waitSemaphoreCount =
          static_cast<uint32_t>(submit_signal_semaphores.size());
      present_info.pWaitSemaphores = submit_signal_semaphores.data();
      present_info.swapchainCount = 1;
      present_info.pSwapchains = &*swapchain_;
      present_info.pImageIndices = &image_index;
      logical_device_.present_queue.presentKHR(present_info);
      ++frame_;
    } catch (vk::OutOfDateKHRError) {
      create_swapchain();
    }
  }
} // namespace imp