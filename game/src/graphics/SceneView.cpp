#include "SceneView.h"

#include <fstream>
#include <iostream>

#include "../system/GpuContext.h"
#include "../util/Align.h"
#include "../util/Math.h"
#include "Scene.h"

namespace imp {
  SceneView::Flyweight::Flyweight(
      gsl::not_null<GpuContext *> context, std::size_t frameCount):
      context_{context},
      frameCount_{frameCount},
      renderPass_{createRenderPass()},
      nonDestructiveRenderPass_{createNonDestructiveRenderPass()},
      skyViewDescriptorSetLayout_{createSkyViewDescriptorSetLayout()},
      primaryDescriptorSetLayout_{createPrimaryDescriptorSetLayout()},
      identityDescriptorSetLayout_{createIdentityDescriptorSetLayout()},
      blurDescriptorSetLayout_{createBlurDescriptorSetLayout()},
      bloomDescriptorSetLayout_{createBloomDescriptorSetLayout()},
      skyViewPipelineLayout_{createSkyViewPipelineLayout()},
      primaryPipelineLayout_{createPrimaryPipelineLayout()},
      identityPipelineLayout_{createIdentityPipelineLayout()},
      blurPipelineLayout_{createBlurPipelineLayout()},
      bloomPipelineLayout_{createBloomPipelineLayout()},
      skyViewPipeline_{createSkyViewPipeline()},
      primaryPipeline_{createPrimaryPipeline()},
      identityPipeline_{createIdentityPipeline()},
      blurPipelines_{createBlurPipelines()},
      bloomPipeline_{createBloomPipeline()},
      generalSampler_{createGeneralSampler()},
      skyViewSampler_{createSkyViewSampler()} {}

  vk::RenderPass SceneView::Flyweight::createRenderPass() const {
    auto attachment = vk::AttachmentDescription{};
    attachment.format = vk::Format::eR16G16B16A16Sfloat;
    attachment.samples = vk::SampleCountFlagBits::e1;
    attachment.loadOp = vk::AttachmentLoadOp::eDontCare;
    attachment.storeOp = vk::AttachmentStoreOp::eStore;
    attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachment.initialLayout = vk::ImageLayout::eUndefined;
    attachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto colorAttachment = vk::AttachmentReference{};
    colorAttachment.attachment = 0;
    colorAttachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = vk::SubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachment;
    auto dependencies = std::array<vk::SubpassDependency, 2>{};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies[0].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies[0].dstAccessMask = vk::AccessFlagBits::eShaderRead;
    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies[1].dstAccessMask = vk::AccessFlagBits::eShaderRead;
    auto createInfo = vk::RenderPassCreateInfo{};
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount =
        static_cast<std::uint32_t>(dependencies.size());
    createInfo.pDependencies = dependencies.data();
    return context_->getDevice().createRenderPass(createInfo);
  }

  vk::RenderPass SceneView::Flyweight::createNonDestructiveRenderPass() const {
    auto attachment = vk::AttachmentDescription{};
    attachment.format = vk::Format::eR16G16B16A16Sfloat;
    attachment.samples = vk::SampleCountFlagBits::e1;
    attachment.loadOp = vk::AttachmentLoadOp::eLoad;
    attachment.storeOp = vk::AttachmentStoreOp::eStore;
    attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachment.initialLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    attachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto colorAttachment = vk::AttachmentReference{};
    colorAttachment.attachment = 0;
    colorAttachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = vk::SubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachment;
    auto dependencies = std::array<vk::SubpassDependency, 2>{};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies[0].dstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[0].srcAccessMask = vk::AccessFlagBits::eShaderRead;
    dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies[1].dstAccessMask = vk::AccessFlagBits::eShaderRead;
    auto createInfo = vk::RenderPassCreateInfo{};
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount =
        static_cast<std::uint32_t>(dependencies.size());
    createInfo.pDependencies = dependencies.data();
    return context_->getDevice().createRenderPass(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createSkyViewDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 3>{};
    bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[1].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    for (auto &binding : bindings) {
      binding.descriptorCount = 1;
      binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    }
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createPrimaryDescriptorSetLayout() const {
    auto bindings = std::array<GpuDescriptorSetLayoutBinding, 4>{};
    bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[1].descriptorType = vk::DescriptorType::eUniformBuffer;
    bindings[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[3].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    for (auto &binding : bindings) {
      binding.descriptorCount = 1;
      binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    }
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = bindings;
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createIdentityDescriptorSetLayout() const {
    auto binding = GpuDescriptorSetLayoutBinding{};
    binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = {&binding, 1};
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createBlurDescriptorSetLayout() const {
    auto binding = GpuDescriptorSetLayoutBinding{};
    binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = {&binding, 1};
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::createBloomDescriptorSetLayout() const {
    auto binding = GpuDescriptorSetLayoutBinding{};
    binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    binding.descriptorCount = 1;
    binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    auto createInfo = GpuDescriptorSetLayoutCreateInfo{};
    createInfo.bindings = {&binding, 1};
    return context_->createDescriptorSetLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createSkyViewPipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&skyViewDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createPrimaryPipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&primaryDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout
  SceneView::Flyweight::createIdentityPipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&identityDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createBlurPipelineLayout() const {
    auto pushConstantRange = GpuPushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.size = 24;
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&blurDescriptorSetLayout_, 1};
    createInfo.pushConstantRanges = {&pushConstantRange, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::PipelineLayout SceneView::Flyweight::createBloomPipelineLayout() const {
    auto createInfo = GpuPipelineLayoutCreateInfo{};
    createInfo.setLayouts = {&bloomDescriptorSetLayout_, 1};
    return context_->createPipelineLayout(createInfo);
  }

  vk::Pipeline SceneView::Flyweight::createSkyViewPipeline() const {
    auto vertModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/GenericVert.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      vertModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto fragModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/SkyViewFrag.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      fragModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto stages = std::array<vk::PipelineShaderStageCreateInfo, 2>{};
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = *vertModule;
    stages[0].pName = "main";
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = *fragModule;
    stages[1].pName = "main";
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &attachment;
    auto dynamicStates =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
    dynamicState.dynamicStateCount =
        static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizationState;
    createInfo.pMultisampleState = &multisampleState;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = skyViewPipelineLayout_;
    createInfo.renderPass = renderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineIndex = -1;
    return context_->getDevice().createGraphicsPipeline({}, createInfo).value;
  }

  vk::Pipeline SceneView::Flyweight::createPrimaryPipeline() const {
    auto vertModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/GenericVert.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      vertModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto fragModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/PrimaryFrag.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      fragModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto stages = std::array<vk::PipelineShaderStageCreateInfo, 2>{};
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = *vertModule;
    stages[0].pName = "main";
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = *fragModule;
    stages[1].pName = "main";
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &attachment;
    auto dynamicStates =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
    dynamicState.dynamicStateCount =
        static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizationState;
    createInfo.pMultisampleState = &multisampleState;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = primaryPipelineLayout_;
    createInfo.renderPass = renderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineIndex = -1;
    return context_->getDevice().createGraphicsPipeline({}, createInfo).value;
  }

  vk::Pipeline SceneView::Flyweight::createIdentityPipeline() const {
    auto vertModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/GenericVert.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      vertModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto fragModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/IdentityFrag.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      fragModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto stages = std::array<vk::PipelineShaderStageCreateInfo, 2>{};
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = *vertModule;
    stages[0].pName = "main";
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = *fragModule;
    stages[1].pName = "main";
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &attachment;
    auto dynamicStates =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
    dynamicState.dynamicStateCount =
        static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizationState;
    createInfo.pMultisampleState = &multisampleState;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = identityPipelineLayout_;
    createInfo.renderPass = renderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineIndex = -1;
    return context_->getDevice().createGraphicsPipeline({}, createInfo).value;
  }

  absl::flat_hash_map<int, vk::Pipeline>
  SceneView::Flyweight::createBlurPipelines() const {
    auto vertModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/GenericVert.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      vertModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto fragModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/BlurFrag.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      fragModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto mapEntry = vk::SpecializationMapEntry{};
    mapEntry.constantID = 0;
    mapEntry.offset = 0;
    mapEntry.size = 4;
    auto specializationInfo = vk::SpecializationInfo{};
    specializationInfo.mapEntryCount = 1;
    specializationInfo.pMapEntries = &mapEntry;
    specializationInfo.dataSize = 4;
    specializationInfo.pData = nullptr;
    auto stages = std::array<vk::PipelineShaderStageCreateInfo, 2>{};
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = *vertModule;
    stages[0].pName = "main";
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = *fragModule;
    stages[1].pName = "main";
    stages[1].pSpecializationInfo = &specializationInfo;
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &attachment;
    auto dynamicStates =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
    dynamicState.dynamicStateCount =
        static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizationState;
    createInfo.pMultisampleState = &multisampleState;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = blurPipelineLayout_;
    createInfo.renderPass = renderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineIndex = -1;
    auto pipelines = absl::flat_hash_map<int, vk::Pipeline>{};
    for (auto kernelSize : std::vector<std::int32_t>{
             3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25}) {
      specializationInfo.pData = &kernelSize;
      pipelines.emplace(
          kernelSize,
          context_->getDevice().createGraphicsPipeline({}, createInfo).value);
    }
    return pipelines;
  }

  vk::Pipeline SceneView::Flyweight::createBloomPipeline() const {
    auto vertModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/GenericVert.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      vertModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto fragModule = vk::UniqueShaderModule{};
    {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open("./data/BloomFrag.spv", std::ios::binary);
      in.seekg(0, std::ios::end);
      code.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto createInfo = vk::ShaderModuleCreateInfo{};
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      fragModule = context_->getDevice().createShaderModuleUnique(createInfo);
    }
    auto stages = std::array<vk::PipelineShaderStageCreateInfo, 2>{};
    stages[0].stage = vk::ShaderStageFlagBits::eVertex;
    stages[0].module = *vertModule;
    stages[0].pName = "main";
    stages[1].stage = vk::ShaderStageFlagBits::eFragment;
    stages[1].module = *fragModule;
    stages[1].pName = "main";
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    auto attachment = vk::PipelineColorBlendAttachmentState{};
    attachment.blendEnable = true;
    attachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    attachment.dstColorBlendFactor = vk::BlendFactor::eOne;
    attachment.colorBlendOp = vk::BlendOp::eAdd;
    attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    attachment.alphaBlendOp = vk::BlendOp::eAdd;
    attachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &attachment;
    auto dynamicStates =
        std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
    dynamicState.dynamicStateCount =
        static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = &vertexInputState;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizationState;
    createInfo.pMultisampleState = &multisampleState;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = bloomPipelineLayout_;
    createInfo.renderPass = renderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineIndex = -1;
    return context_->getDevice().createGraphicsPipeline({}, createInfo).value;
  }

  vk::Sampler SceneView::Flyweight::createGeneralSampler() const {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  vk::Sampler SceneView::Flyweight::createSkyViewSampler() const {
    auto createInfo = GpuSamplerCreateInfo{};
    createInfo.magFilter = vk::Filter::eLinear;
    createInfo.minFilter = vk::Filter::eLinear;
    createInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    return context_->createSampler(createInfo);
  }

  SceneView::Flyweight::~Flyweight() {
    auto device = context_->getDevice();
    device.destroy(bloomPipeline_);
    for (auto [_, pipeline] : blurPipelines_) {
      device.destroy(pipeline);
    }
    device.destroy(identityPipeline_);
    device.destroy(primaryPipeline_);
    device.destroy(skyViewPipeline_);
    device.destroy(nonDestructiveRenderPass_);
    device.destroy(renderPass_);
  }

  gsl::not_null<GpuContext *>
  SceneView::Flyweight::getContext() const noexcept {
    return context_;
  }

  std::size_t SceneView::Flyweight::getFrameCount() const noexcept {
    return frameCount_;
  }

  vk::RenderPass SceneView::Flyweight::getRenderPass() const noexcept {
    return renderPass_;
  }

  vk::RenderPass
  SceneView::Flyweight::getNonDestructiveRenderPass() const noexcept {
    return nonDestructiveRenderPass_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getSkyViewDescriptorSetLayout() const noexcept {
    return skyViewDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getPrimaryDescriptorSetLayout() const noexcept {
    return primaryDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getIdentityDescriptorSetLayout() const noexcept {
    return identityDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getBlurDescriptorSetLayout() const noexcept {
    return blurDescriptorSetLayout_;
  }

  vk::DescriptorSetLayout
  SceneView::Flyweight::getBloomDescriptorSetLayout() const {
    return bloomDescriptorSetLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getSkyViewPipelineLayout() const noexcept {
    return skyViewPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getPrimaryPipelineLayout() const noexcept {
    return primaryPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getIdentityPipelineLayout() const noexcept {
    return identityPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getBlurPipelineLayout() const noexcept {
    return blurPipelineLayout_;
  }

  vk::PipelineLayout
  SceneView::Flyweight::getBloomPipelineLayout() const noexcept {
    return bloomPipelineLayout_;
  }

  vk::Pipeline SceneView::Flyweight::getSkyViewPipeline() const noexcept {
    return skyViewPipeline_;
  }

  vk::Pipeline SceneView::Flyweight::getPrimaryPipeline() const noexcept {
    return primaryPipeline_;
  }

  vk::Pipeline SceneView::Flyweight::getIdentityPipeline() const noexcept {
    return identityPipeline_;
  }

  vk::Pipeline
  SceneView::Flyweight::getBlurPipeline(int kernelSize) const noexcept {
    return blurPipelines_.at(kernelSize);
  }

  vk::Pipeline SceneView::Flyweight::getBloomPipeline() const noexcept {
    return bloomPipeline_;
  }

  vk::Sampler SceneView::Flyweight::getSkyViewSampler() const noexcept {
    return skyViewSampler_;
  }

  vk::Sampler SceneView::Flyweight::getGeneralSampler() const noexcept {
    return generalSampler_;
  }

  SceneView::Frame::Frame(
      GpuImage &&renderImage,
      GpuImage &&smallBloomImageArray,
      GpuImage &&mediumBloomImageArray,
      GpuImage &&largeBloomImageArray):
      renderImage{std::move(renderImage)},
      smallBloomImageArray{std::move(smallBloomImageArray)},
      mediumBloomImageArray{std::move(mediumBloomImageArray)},
      largeBloomImageArray{std::move(largeBloomImageArray)} {}

  SceneView::SceneView(
      gsl::not_null<Flyweight const *> flyweight,
      gsl::not_null<std::shared_ptr<Scene>> scene,
      Extent2u const &extent) noexcept:
      flyweight_{flyweight},
      scene_{std::move(scene)},
      extent_{extent},
      descriptorPool_{createDescriptorPool()},
      uniformBuffer_{createUniformBuffer()},
      skyViewImage_{createSkyViewImage()},
      frames_{createFrames()},
      viewMatrix_{Eigen::Matrix4f::Identity()},
      invViewMatrix_{Eigen::Matrix4f::Identity()},
      projectionMatrix_{Eigen::Matrix4f::Identity()},
      invProjectionMatrix_{Eigen::Matrix4f::Identity()},
      exposure_{1.0f},
      smallBloomKernel_{15},
      mediumBloomKernel_{19},
      largeBloomKernel_{23},
      smallBloomWeight_{0.0030f},
      mediumBloomWeight_{0.0025f},
      largeBloomWeight_{0.0020f},
      firstFrame_{true} {
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      auto &frame = frames_[i];
      initSkyViewImageView(i);
      initRenderImageView(i);
      initHalfRenderImageView(i);
      initFourthRenderImageView(i);
      initEighthRenderImageView(i);
      initSmallBloomImageViews(i);
      initMediumBloomImageViews(i);
      initLargeBloomImageViews(i);
      initSkyViewFramebuffer(frame);
      initPrimaryFramebuffer(frame);
      initHalfPrimaryFramebuffer(frame);
      initFourthPrimaryFramebuffer(frame);
      initEighthPrimaryFramebuffer(frame);
      initSmallBloomFramebuffers(frame);
      initMediumBloomFramebuffers(frame);
      initLargeBloomFramebuffers(frame);
      allocateDescriptorSets(i);
      initSkyViewDescriptorSet(i);
      initPrimaryDescriptorSet(i);
      initIdentityDescriptorSets(i);
      initSmallBlurDescriptorSets(i);
      initMediumBlurDescriptorSets(i);
      initLargeBlurDescriptorSets(i);
      initBloomDescriptorSets(i);
      initCommandPool(i);
      initCommandBuffers(i);
      initSemaphores(i);
    }
  }

  vk::DescriptorPool SceneView::createDescriptorPool() const {
    auto frameCount32 = static_cast<std::uint32_t>(flyweight_->getFrameCount());
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
        // sky view
        {vk::DescriptorType::eUniformBuffer, 2 * frameCount32},
        {vk::DescriptorType::eCombinedImageSampler, 1 * frameCount32},
        // primary
        {vk::DescriptorType::eUniformBuffer, 2 * frameCount32},
        {vk::DescriptorType::eCombinedImageSampler, 2 * frameCount32},
        // identity
        {vk::DescriptorType::eCombinedImageSampler, 3 * frameCount32},
        // identity blend
        {vk::DescriptorType::eCombinedImageSampler, 3 * frameCount32},
        // blur
        {vk::DescriptorType::eCombinedImageSampler, 6 * frameCount32}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = 14 * frameCount32;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    return flyweight_->getContext()->getDevice().createDescriptorPool(
        createInfo);
  }

  GpuBuffer SceneView::createUniformBuffer() const {
    auto frameCount = flyweight_->getFrameCount();
    auto buffer = vk::BufferCreateInfo{};
    buffer.size = frameCount != 1 ? UNIFORM_BUFFER_STRIDE * frameCount
                                  : UNIFORM_BUFFER_SIZE;
    buffer.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    allocation.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    return GpuBuffer{
        flyweight_->getContext()->getAllocator(), buffer, allocation};
  }

  GpuImage SceneView::createSkyViewImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = SKY_VIEW_IMAGE_EXTENT;
    image.mipLevels = 1;
    image.arrayLayers = static_cast<std::uint32_t>(flyweight_->getFrameCount());
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage = vk::ImageUsageFlagBits::eSampled |
                  vk::ImageUsageFlagBits::eColorAttachment;
    image.sharingMode = vk::SharingMode::eExclusive;
    image.initialLayout = vk::ImageLayout::eUndefined;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  std::vector<SceneView::Frame> SceneView::createFrames() const {
    auto frames = std::vector<SceneView::Frame>{};
    frames.reserve(flyweight_->getFrameCount());
    for (auto i = std::size_t{}; i < frames.capacity(); ++i) {
      frames.emplace_back(
          createRenderImage(),
          createSmallBloomImageArray(),
          createMediumBloomImageArray(),
          createLargeBloomImageArray());
    }
    return frames;
  }

  GpuImage SceneView::createRenderImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width, extent_.height, 1};
    image.mipLevels = 4;
    image.arrayLayers = 1;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage = vk::ImageUsageFlagBits::eSampled |
                  vk::ImageUsageFlagBits::eColorAttachment;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  GpuImage SceneView::createSmallBloomImageArray() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width / 2, extent_.height / 2, 1};
    image.mipLevels = 1;
    image.arrayLayers = 2;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage = vk::ImageUsageFlagBits::eSampled |
                  vk::ImageUsageFlagBits::eColorAttachment;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  GpuImage SceneView::createMediumBloomImageArray() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width / 4, extent_.height / 4, 1};
    image.mipLevels = 1;
    image.arrayLayers = 2;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage = vk::ImageUsageFlagBits::eSampled |
                  vk::ImageUsageFlagBits::eColorAttachment;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  GpuImage SceneView::createLargeBloomImageArray() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width / 8, extent_.height / 8, 1};
    image.mipLevels = 1;
    image.arrayLayers = 2;
    image.samples = vk::SampleCountFlagBits::e1;
    image.tiling = vk::ImageTiling::eOptimal;
    image.usage = vk::ImageUsageFlagBits::eSampled |
                  vk::ImageUsageFlagBits::eColorAttachment;
    auto allocation = VmaAllocationCreateInfo{};
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    return GpuImage{
        flyweight_->getContext()->getAllocator(), image, allocation};
  }

  void SceneView::initSkyViewImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = skyViewImage_.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = skyViewImage_.getFormat();
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].skyViewImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].renderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initHalfRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 1;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].halfRenderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initFourthRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 2;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].fourthRenderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initEighthRenderImageView(std::size_t i) {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frames_[i].renderImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frames_[i].renderImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 3;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frames_[i].eighthRenderImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initSmallBloomImageViews(std::size_t i) {
    auto &frame = frames_[i];
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.smallBloomImageArray.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.smallBloomImageArray.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    for (auto j = 0; j < 2; ++j) {
      createInfo.subresourceRange.baseArrayLayer = j;
      frame.smallBloomImageViews[j] =
          flyweight_->getContext()->getDevice().createImageView(createInfo);
    }
  }

  void SceneView::initMediumBloomImageViews(std::size_t i) {
    auto &frame = frames_[i];
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.mediumBloomImageArray.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.mediumBloomImageArray.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    for (auto j = 0; j < 2; ++j) {
      createInfo.subresourceRange.baseArrayLayer = j;
      frame.mediumBloomImageViews[j] =
          flyweight_->getContext()->getDevice().createImageView(createInfo);
    }
  }

  void SceneView::initLargeBloomImageViews(std::size_t i) {
    auto &frame = frames_[i];
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.largeBloomImageArray.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.largeBloomImageArray.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    for (auto j = 0; j < 2; ++j) {
      createInfo.subresourceRange.baseArrayLayer = j;
      frame.largeBloomImageViews[j] =
          flyweight_->getContext()->getDevice().createImageView(createInfo);
    }
  }

  void SceneView::initSkyViewFramebuffer(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &frame.skyViewImageView;
    createInfo.width = skyViewImage_.getExtent().width;
    createInfo.height = skyViewImage_.getExtent().height;
    createInfo.layers = 1;
    frame.skyViewFramebuffer =
        flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
  }

  void SceneView::initPrimaryFramebuffer(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &frame.renderImageView;
    createInfo.width = frame.renderImage.getExtent().width;
    createInfo.height = frame.renderImage.getExtent().height;
    createInfo.layers = 1;
    frame.primaryFramebuffer =
        flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
  }

  void SceneView::initHalfPrimaryFramebuffer(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &frame.halfRenderImageView;
    createInfo.width = frame.renderImage.getExtent().width / 2;
    createInfo.height = frame.renderImage.getExtent().height / 2;
    createInfo.layers = 1;
    frame.halfPrimaryFramebuffer =
        flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
  }

  void SceneView::initFourthPrimaryFramebuffer(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &frame.fourthRenderImageView;
    createInfo.width = frame.renderImage.getExtent().width / 4;
    createInfo.height = frame.renderImage.getExtent().height / 4;
    createInfo.layers = 1;
    frame.fourthPrimaryFramebuffer =
        flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
  }

  void SceneView::initEighthPrimaryFramebuffer(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &frame.eighthRenderImageView;
    createInfo.width = frame.renderImage.getExtent().width / 8;
    createInfo.height = frame.renderImage.getExtent().height / 8;
    createInfo.layers = 1;
    frame.eighthPrimaryFramebuffer =
        flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
  }

  void SceneView::initSmallBloomFramebuffers(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.width = frame.smallBloomImageArray.getExtent().width;
    createInfo.height = frame.smallBloomImageArray.getExtent().height;
    createInfo.layers = 1;
    for (auto i = 0; i < 2; ++i) {
      createInfo.pAttachments = &frame.smallBloomImageViews[i];
      frame.smallBloomFramebuffers[i] =
          flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
    }
  }

  void SceneView::initMediumBloomFramebuffers(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.width = frame.mediumBloomImageArray.getExtent().width;
    createInfo.height = frame.mediumBloomImageArray.getExtent().height;
    createInfo.layers = 1;
    for (auto i = 0; i < 2; ++i) {
      createInfo.pAttachments = &frame.mediumBloomImageViews[i];
      frame.mediumBloomFramebuffers[i] =
          flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
    }
  }

  void SceneView::initLargeBloomFramebuffers(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.width = frame.largeBloomImageArray.getExtent().width;
    createInfo.height = frame.largeBloomImageArray.getExtent().height;
    createInfo.layers = 1;
    for (auto i = 0; i < 2; ++i) {
      createInfo.pAttachments = &frame.largeBloomImageViews[i];
      frame.largeBloomFramebuffers[i] =
          flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
    }
  }

  void SceneView::allocateDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    auto skyViewSetLayout = flyweight_->getSkyViewDescriptorSetLayout();
    auto primarySetLayout = flyweight_->getPrimaryDescriptorSetLayout();
    auto identitySetLayout = flyweight_->getIdentityDescriptorSetLayout();
    auto blurSetLayout = flyweight_->getBlurDescriptorSetLayout();
    auto bloomSetLayout = flyweight_->getBloomDescriptorSetLayout();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &skyViewSetLayout;
    device.allocateDescriptorSets(&allocateInfo, &frame.skyViewDescriptorSet);
    allocateInfo.pSetLayouts = &primarySetLayout;
    device.allocateDescriptorSets(&allocateInfo, &frame.primaryDescriptorSet);
    allocateInfo.pSetLayouts = &identitySetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.identityDescriptorSets[0]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.identityDescriptorSets[1]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.identityDescriptorSets[2]);
    allocateInfo.pSetLayouts = &blurSetLayout;
    device.allocateDescriptorSets(
        &allocateInfo, &frame.smallBlurDescriptorSets[0]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.smallBlurDescriptorSets[1]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.mediumBlurDescriptorSets[0]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.mediumBlurDescriptorSets[1]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.largeBlurDescriptorSets[0]);
    device.allocateDescriptorSets(
        &allocateInfo, &frame.largeBlurDescriptorSets[1]);
    allocateInfo.pSetLayouts = &bloomSetLayout;
    device.allocateDescriptorSets(&allocateInfo, &frame.bloomDescriptorSets[0]);
    device.allocateDescriptorSets(&allocateInfo, &frame.bloomDescriptorSets[1]);
    device.allocateDescriptorSets(&allocateInfo, &frame.bloomDescriptorSets[2]);
  }

  void SceneView::initSkyViewDescriptorSet(std::size_t i) {
    auto &frame = frames_[i];
    auto info = vk::DescriptorBufferInfo{};
    info.buffer = uniformBuffer_.get();
    info.offset = UNIFORM_BUFFER_STRIDE * i;
    info.range = UNIFORM_BUFFER_SIZE;
    auto write = vk::WriteDescriptorSet{};
    write.dstSet = frame.skyViewDescriptorSet;
    write.dstBinding = 1;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eUniformBuffer;
    write.pBufferInfo = &info;
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
  }

  void SceneView::initPrimaryDescriptorSet(std::size_t i) {
    auto &frame = frames_[i];
    auto sceneViewBufferInfo = vk::DescriptorBufferInfo{};
    sceneViewBufferInfo.buffer = uniformBuffer_.get();
    sceneViewBufferInfo.offset = UNIFORM_BUFFER_STRIDE * i;
    sceneViewBufferInfo.range = UNIFORM_BUFFER_SIZE;
    auto skyViewTextureInfo = vk::DescriptorImageInfo{};
    skyViewTextureInfo.sampler = flyweight_->getSkyViewSampler();
    skyViewTextureInfo.imageView = frame.skyViewImageView;
    skyViewTextureInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto writes = std::array<vk::WriteDescriptorSet, 2>{};
    writes[0].dstSet = frame.primaryDescriptorSet;
    writes[0].dstBinding = 1;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &sceneViewBufferInfo;
    writes[1].dstSet = frame.primaryDescriptorSet;
    writes[1].dstBinding = 3;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writes[1].pImageInfo = &skyViewTextureInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(writes, {});
  }

  void SceneView::initIdentityDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto info = vk::DescriptorImageInfo{};
    info.sampler = flyweight_->getGeneralSampler();
    info.imageView = frame.renderImageView;
    info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto write = vk::WriteDescriptorSet{};
    write.dstSet = frame.identityDescriptorSets[0];
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.pImageInfo = &info;
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
    info.imageView = frame.halfRenderImageView;
    write.dstSet = frame.identityDescriptorSets[1];
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
    info.imageView = frame.fourthRenderImageView;
    write.dstSet = frame.identityDescriptorSets[2];
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
  }

  void SceneView::initSmallBlurDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    {
      auto info = vk::DescriptorImageInfo{};
      info.sampler = flyweight_->getGeneralSampler();
      info.imageView = frame.halfRenderImageView;
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto write = vk::WriteDescriptorSet{};
      write.dstSet = frame.smallBlurDescriptorSets[0];
      write.dstBinding = 0;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo = &info;
      device.updateDescriptorSets(write, 0);
    }
    {
      auto info = vk::DescriptorImageInfo{};
      info.sampler = flyweight_->getGeneralSampler();
      info.imageView = frame.smallBloomImageViews[0];
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto write = vk::WriteDescriptorSet{};
      write.dstSet = frame.smallBlurDescriptorSets[1];
      write.dstBinding = 0;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo = &info;
      device.updateDescriptorSets(write, 0);
    }
  }

  void SceneView::initMediumBlurDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    {
      auto info = vk::DescriptorImageInfo{};
      info.sampler = flyweight_->getGeneralSampler();
      info.imageView = frame.fourthRenderImageView;
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto write = vk::WriteDescriptorSet{};
      write.dstSet = frame.mediumBlurDescriptorSets[0];
      write.dstBinding = 0;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo = &info;
      device.updateDescriptorSets(write, 0);
    }
    {
      auto info = vk::DescriptorImageInfo{};
      info.sampler = flyweight_->getGeneralSampler();
      info.imageView = frame.mediumBloomImageViews[0];
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto write = vk::WriteDescriptorSet{};
      write.dstSet = frame.mediumBlurDescriptorSets[1];
      write.dstBinding = 0;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo = &info;
      device.updateDescriptorSets(write, 0);
    }
  }

  void SceneView::initLargeBlurDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    {
      auto info = vk::DescriptorImageInfo{};
      info.sampler = flyweight_->getGeneralSampler();
      info.imageView = frame.eighthRenderImageView;
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto write = vk::WriteDescriptorSet{};
      write.dstSet = frame.largeBlurDescriptorSets[0];
      write.dstBinding = 0;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo = &info;
      device.updateDescriptorSets(write, 0);
    }
    {
      auto info = vk::DescriptorImageInfo{};
      info.sampler = flyweight_->getGeneralSampler();
      info.imageView = frame.largeBloomImageViews[0];
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      auto write = vk::WriteDescriptorSet{};
      write.dstSet = frame.largeBlurDescriptorSets[1];
      write.dstBinding = 0;
      write.dstArrayElement = 0;
      write.descriptorCount = 1;
      write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo = &info;
      device.updateDescriptorSets(write, 0);
    }
  }

  void SceneView::initBloomDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto info = vk::DescriptorImageInfo{};
    info.sampler = flyweight_->getGeneralSampler();
    info.imageView = frame.largeBloomImageViews[1];
    info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto write = vk::WriteDescriptorSet{};
    write.dstSet = frame.bloomDescriptorSets[0];
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.pImageInfo = &info;
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
    info.imageView = frame.mediumBloomImageViews[1];
    write.dstSet = frame.bloomDescriptorSets[1];
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
    info.imageView = frame.smallBloomImageViews[1];
    write.dstSet = frame.bloomDescriptorSets[2];
    flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
  }

  void SceneView::initCommandPool(std::size_t i) {
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    createInfo.queueFamilyIndex = flyweight_->getContext()->getComputeFamily();
    frames_[i].commandPool =
        flyweight_->getContext()->getDevice().createCommandPool(createInfo);
  }

  void SceneView::initCommandBuffers(std::size_t i) {
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandPool = frames_[i].commandPool;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = 1;
    auto device = flyweight_->getContext()->getDevice();
    device.allocateCommandBuffers(&allocateInfo, &frames_[i].commandBuffer);
  }

  void SceneView::initSemaphores(std::size_t i) {
    auto device = flyweight_->getContext()->getDevice();
    frames_[i].semaphore = device.createSemaphore({});
  }

  SceneView::~SceneView() {
    auto device = flyweight_->getContext()->getDevice();
    for (auto &frame : frames_) {
      device.destroy(frame.semaphore);
      device.destroy(frame.commandPool);
      for (auto framebuffer : frame.largeBloomFramebuffers) {
        device.destroy(framebuffer);
      }
      for (auto framebuffer : frame.mediumBloomFramebuffers) {
        device.destroy(framebuffer);
      }
      for (auto framebuffer : frame.smallBloomFramebuffers) {
        device.destroy(framebuffer);
      }
      device.destroy(frame.eighthPrimaryFramebuffer);
      device.destroy(frame.fourthPrimaryFramebuffer);
      device.destroy(frame.halfPrimaryFramebuffer);
      device.destroy(frame.primaryFramebuffer);
      device.destroy(frame.skyViewFramebuffer);
      for (auto imageView : frame.largeBloomImageViews) {
        device.destroy(imageView);
      }
      for (auto imageView : frame.mediumBloomImageViews) {
        device.destroy(imageView);
      }
      for (auto imageView : frame.smallBloomImageViews) {
        device.destroy(imageView);
      }
      device.destroy(frame.eighthRenderImageView);
      device.destroy(frame.fourthRenderImageView);
      device.destroy(frame.halfRenderImageView);
      device.destroy(frame.renderImageView);
      device.destroy(frame.skyViewImageView);
    }
    device.destroy(descriptorPool_);
  }

  void SceneView::render(std::size_t i) {
    auto &context = *flyweight_->getContext();
    auto queue = context.getComputeQueue();
    auto device = context.getDevice();
    auto &frame = frames_[i];
    updateUniformBuffer(i);
    if (frame.renderImage.getExtent() !=
        Extent3u{extent_.width, extent_.height, 1}) {
      updateRenderImages(i);
    }
    if (frame.scene != scene_) {
      updateSkyViewDescriptorSet(i);
      updatePrimaryDescriptorSet(i);
      frame.scene = scene_;
    }
    device.resetCommandPool(frame.commandPool);
    submitCommands(i);
    firstFrame_ = false;
  }

  void SceneView::updateRenderImages(std::size_t i) {
    auto device = flyweight_->getContext()->getDevice();
    auto &frame = frames_[i];
    for (auto framebuffer : frame.largeBloomFramebuffers) {
      device.destroy(framebuffer);
    }
    for (auto framebuffer : frame.mediumBloomFramebuffers) {
      device.destroy(framebuffer);
    }
    for (auto framebuffer : frame.smallBloomFramebuffers) {
      device.destroy(framebuffer);
    }
    for (auto imageView : frame.largeBloomImageViews) {
      device.destroy(imageView);
    }
    for (auto imageView : frame.mediumBloomImageViews) {
      device.destroy(imageView);
    }
    for (auto imageView : frame.smallBloomImageViews) {
      device.destroy(imageView);
    }
    device.destroy(frame.eighthRenderImageView);
    device.destroy(frame.fourthRenderImageView);
    device.destroy(frame.halfRenderImageView);
    device.destroy(frame.renderImageView);
    frame.renderImage = createRenderImage();
    frame.smallBloomImageArray = createSmallBloomImageArray();
    frame.mediumBloomImageArray = createMediumBloomImageArray();
    frame.largeBloomImageArray = createLargeBloomImageArray();
    initRenderImageView(i);
    initHalfRenderImageView(i);
    initFourthRenderImageView(i);
    initEighthRenderImageView(i);
    initSmallBloomImageViews(i);
    initMediumBloomImageViews(i);
    initLargeBloomImageViews(i);
    initPrimaryFramebuffer(frame);
    initHalfPrimaryFramebuffer(frame);
    initFourthPrimaryFramebuffer(frame);
    initEighthPrimaryFramebuffer(frame);
    initSmallBloomFramebuffers(frame);
    initMediumBloomFramebuffers(frame);
    initLargeBloomFramebuffers(frame);
    initIdentityDescriptorSets(i);
    initSmallBlurDescriptorSets(i);
    initMediumBlurDescriptorSets(i);
    initLargeBlurDescriptorSets(i);
    initBloomDescriptorSets(i);
  }

  void SceneView::updateUniformBuffer(std::size_t frameIndex) {
    using Eigen::Matrix4f;
    using Eigen::Vector3f;
    using Eigen::Vector4f;
    Vector3f position = invViewMatrix_.col(3).head<3>();
    Vector3f zenith = position - scene_->getPlanet()->getPosition();
    auto radius = zenith.norm();
    auto altitude = radius - scene_->getPlanet()->getGroundRadius();
    zenith /= radius;
    Vector3f tangent =
        std::abs(zenith.x()) > std::abs(zenith.y())
            ? Vector3f{-zenith.z(), 0.0f, zenith.x()} /
                  std::sqrt(zenith.x() * zenith.x() + zenith.z() * zenith.z())
            : Vector3f{0.0f, zenith.z(), -zenith.y()} /
                  std::sqrt(zenith.y() * zenith.y() + zenith.z() * zenith.z());
    Vector3f bitangent = zenith.cross(tangent);
    Matrix4f skyViewMatrix = Matrix4f::Identity();
    skyViewMatrix.col(0).head<3>() = tangent;
    skyViewMatrix.col(1).head<3>() = bitangent;
    skyViewMatrix.col(2).head<3>() = zenith;
    skyViewMatrix.col(3).head<3>() = position;
    skyViewMatrix = skyViewMatrix.inverse().eval();
    Matrix4f viewDirectionMatrix = Eigen::Matrix4f::Identity();
    viewDirectionMatrix.topLeftCorner<3, 3>() =
        skyViewMatrix.topLeftCorner<3, 3>() *
        invViewMatrix_.topLeftCorner<3, 3>();
    viewDirectionMatrix *= invProjectionMatrix_;
    auto viewDirections = std::array{
        Vector4f{-1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{-1.0f, -1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, -1.0f, 1.0f, 1.0f}};
    for (auto &viewDirection : viewDirections) {
      viewDirection = viewDirectionMatrix * viewDirection;
      viewDirection *= 1.0f / viewDirection.w();
      viewDirection.head<3>().normalize();
    }
    Vector3f sunDirection = skyViewMatrix.topLeftCorner<3, 3>() *
                            scene_->getSunLight()->getDirection();
    auto groundLat =
        -std::acos(scene_->getPlanet()->getGroundRadius() / radius);
    auto atmosphereLat =
        scene_->getPlanet()->getAtmosphereRadius() < radius
            ? -std::acos(scene_->getPlanet()->getAtmosphereRadius() / radius)
            : 0.5f * PI<float>;
    auto offset = UNIFORM_BUFFER_STRIDE * frameIndex;
    auto data = uniformBuffer_.getMappedData() + offset;
    std::memcpy(data + 0, &viewDirections[0], 12);
    std::memcpy(data + 16, &viewDirections[1], 12);
    std::memcpy(data + 32, &viewDirections[2], 12);
    std::memcpy(data + 48, &viewDirections[3], 12);
    std::memcpy(data + 64, &sunDirection, 12);
    std::memcpy(data + 76, &altitude, 4);
    std::memcpy(data + 80, &groundLat, 4);
    std::memcpy(data + 84, &atmosphereLat, 4);
    std::memcpy(data + 88, &exposure_, 4);
    uniformBuffer_.flush(offset, UNIFORM_BUFFER_SIZE);
  }

  void SceneView::updateSkyViewDescriptorSet(std::size_t i) {
    auto sceneBufferInfo = vk::DescriptorBufferInfo{};
    sceneBufferInfo.buffer = scene_->getUniformBuffer().get();
    sceneBufferInfo.offset = Scene::UNIFORM_BUFFER_STRIDE * i;
    sceneBufferInfo.range = Scene::UNIFORM_BUFFER_SIZE;
    auto transmittanceImageInfo = vk::DescriptorImageInfo{};
    transmittanceImageInfo.sampler =
        scene_->getFlyweight()->getTransmittanceSampler();
    transmittanceImageInfo.imageView = scene_->getTransmittanceImageView(i);
    transmittanceImageInfo.imageLayout =
        vk::ImageLayout::eShaderReadOnlyOptimal;
    auto writes = std::array<vk::WriteDescriptorSet, 2>{};
    writes[0].dstSet = frames_[i].skyViewDescriptorSet;
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    writes[0].pBufferInfo = &sceneBufferInfo;
    writes[1].dstSet = frames_[i].skyViewDescriptorSet;
    writes[1].dstBinding = 2;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writes[1].pImageInfo = &transmittanceImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(writes, {});
  }

  void SceneView::updatePrimaryDescriptorSet(std::size_t i) {
    auto sceneBufferInfo = vk::DescriptorBufferInfo{};
    sceneBufferInfo.buffer = scene_->getUniformBuffer().get();
    sceneBufferInfo.offset = Scene::UNIFORM_BUFFER_STRIDE * i;
    sceneBufferInfo.range = Scene::UNIFORM_BUFFER_SIZE;
    auto sceneBufferWrite = vk::WriteDescriptorSet{};
    sceneBufferWrite.dstSet = frames_[i].primaryDescriptorSet;
    sceneBufferWrite.dstBinding = 0;
    sceneBufferWrite.dstArrayElement = 0;
    sceneBufferWrite.descriptorCount = 1;
    sceneBufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    sceneBufferWrite.pBufferInfo = &sceneBufferInfo;
    auto transmittanceImageInfo = vk::DescriptorImageInfo{};
    transmittanceImageInfo.sampler =
        scene_->getFlyweight()->getTransmittanceSampler();
    transmittanceImageInfo.imageView = scene_->getTransmittanceImageView(i);
    transmittanceImageInfo.imageLayout =
        vk::ImageLayout::eShaderReadOnlyOptimal;
    auto transmittanceLutWrite = vk::WriteDescriptorSet{};
    transmittanceLutWrite.dstSet = frames_[i].primaryDescriptorSet;
    transmittanceLutWrite.dstBinding = 2;
    transmittanceLutWrite.dstArrayElement = 0;
    transmittanceLutWrite.descriptorCount = 1;
    transmittanceLutWrite.descriptorType =
        vk::DescriptorType::eCombinedImageSampler;
    transmittanceLutWrite.pImageInfo = &transmittanceImageInfo;
    flyweight_->getContext()->getDevice().updateDescriptorSets(
        {sceneBufferWrite, transmittanceLutWrite}, {});
  }

  void SceneView::submitCommands(std::size_t i) {
    auto &frame = frames_[i];
    frame.commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    computeSkyViewImage(i);
    computeRenderImage(i);
    computeRenderImageMips(i);
    computeBloomImageArray(frame);
    applyBloom(i);
    /*{
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask =
          vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = {};
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex =
          flyweight_->getContext()->getComputeFamily();
      barrier.dstQueueFamilyIndex =
          flyweight_->getContext()->getGraphicsFamily();
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eBottomOfPipe,
          {},
          {},
          {},
          barrier);
    }*/
    frame.commandBuffer.end();
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.commandBuffer;
    // submitInfo.signalSemaphoreCount = 1;
    // submitInfo.pSignalSemaphores = &frame.semaphore;
    flyweight_->getContext()->getGraphicsQueue().submit(submitInfo);
  }

  void SceneView::computeSkyViewImage(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.framebuffer = frame.skyViewFramebuffer;
    renderPassBegin.renderArea.extent.width = skyViewImage_.getExtent().width;
    renderPassBegin.renderArea.extent.height = skyViewImage_.getExtent().height;
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getSkyViewPipeline());
    auto viewport = vk::Viewport{};
    viewport.width = renderPassBegin.renderArea.extent.width;
    viewport.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setViewport(0, viewport);
    auto scissor = vk::Rect2D{};
    scissor.extent.width = renderPassBegin.renderArea.extent.width;
    scissor.extent.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getSkyViewPipelineLayout(),
        0,
        frame.skyViewDescriptorSet,
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();
    /*{
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = skyViewImage_.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }*/
    /*frame.commandBuffer.dispatch(
        (skyViewImage_.getExtent().width + 7) / 8,
        (skyViewImage_.getExtent().height + 7) / 8,
        1);*/
  }

  void SceneView::computeRenderImage(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.framebuffer = frame.primaryFramebuffer;
    renderPassBegin.renderArea.extent.width =
        frame.renderImage.getExtent().width;
    renderPassBegin.renderArea.extent.height =
        frame.renderImage.getExtent().height;
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getPrimaryPipeline());
    auto viewport = vk::Viewport{};
    viewport.width = renderPassBegin.renderArea.extent.width;
    viewport.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setViewport(0, viewport);
    auto scissor = vk::Rect2D{};
    scissor.extent.width = renderPassBegin.renderArea.extent.width;
    scissor.extent.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getPrimaryPipelineLayout(),
        0,
        frame.primaryDescriptorSet,
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();
    /*{
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = skyViewImage_.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = static_cast<std::uint32_t>(i);
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }*/
    /*{
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }*/
    /*frame.commandBuffer.dispatch(
        (frame.renderImage.getExtent().width + 7) / 8,
        (frame.renderImage.getExtent().height + 7) / 8,
        1);*/
  }

  void SceneView::computeRenderImageMips(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    auto viewport = vk::Viewport{};
    auto scissor = vk::Rect2D{};
    renderPassBegin.framebuffer = frame.halfPrimaryFramebuffer;
    renderPassBegin.renderArea.extent.width =
        frame.renderImage.getExtent().width / 2;
    renderPassBegin.renderArea.extent.height =
        frame.renderImage.getExtent().height / 2;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getIdentityPipeline());
    viewport.width = renderPassBegin.renderArea.extent.width;
    viewport.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setViewport(0, viewport);
    scissor.extent.width = renderPassBegin.renderArea.extent.width;
    scissor.extent.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getIdentityPipelineLayout(),
        0,
        frame.identityDescriptorSets[0],
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();
    renderPassBegin.framebuffer = frame.fourthPrimaryFramebuffer;
    renderPassBegin.renderArea.extent.width =
        frame.renderImage.getExtent().width / 4;
    renderPassBegin.renderArea.extent.height =
        frame.renderImage.getExtent().height / 4;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getIdentityPipeline());
    viewport.width = renderPassBegin.renderArea.extent.width;
    viewport.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setViewport(0, viewport);
    scissor.extent.width = renderPassBegin.renderArea.extent.width;
    scissor.extent.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getIdentityPipelineLayout(),
        0,
        frame.identityDescriptorSets[1],
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();
    renderPassBegin.framebuffer = frame.eighthPrimaryFramebuffer;
    renderPassBegin.renderArea.extent.width =
        frame.renderImage.getExtent().width / 8;
    renderPassBegin.renderArea.extent.height =
        frame.renderImage.getExtent().height / 8;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getIdentityPipeline());
    viewport.width = renderPassBegin.renderArea.extent.width;
    viewport.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setViewport(0, viewport);
    scissor.extent.width = renderPassBegin.renderArea.extent.width;
    scissor.extent.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getIdentityPipelineLayout(),
        0,
        frame.identityDescriptorSets[2],
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();

    /* auto barrier = vk::ImageMemoryBarrier{};
    barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    barrier.oldLayout = vk::ImageLayout::eGeneral;
    barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = frame.renderImage.get();
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barrier);
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.subresourceRange.baseMipLevel = 1;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barrier);
    auto region = vk::ImageBlit{};
    region.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.srcSubresource.mipLevel = 0;
    region.srcSubresource.baseArrayLayer = 0;
    region.srcSubresource.layerCount = 1;
    region.srcOffsets[0].x = 0;
    region.srcOffsets[0].y = 0;
    region.srcOffsets[1].x = extent_.width;
    region.srcOffsets[1].y = extent_.height;
    region.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.dstSubresource.mipLevel = 1;
    region.dstSubresource.baseArrayLayer = 0;
    region.dstSubresource.layerCount = 1;
    region.dstOffsets[0].x = 0;
    region.dstOffsets[0].y = 0;
    region.dstOffsets[1].x = extent_.width / 2;
    region.dstOffsets[1].y = extent_.height / 2;
    frame.commandBuffer.blitImage(
        frame.renderImage.get(),
        vk::ImageLayout::eTransferSrcOptimal,
        frame.renderImage.get(),
        vk::ImageLayout::eTransferDstOptimal,
        region,
        vk::Filter::eLinear);
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
    barrier.subresourceRange.baseMipLevel = 1;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barrier);
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.subresourceRange.baseMipLevel = 2;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barrier);
    region.srcSubresource.mipLevel = 1;
    region.srcOffsets[1].x = extent_.width / 2;
    region.srcOffsets[1].y = extent_.height / 2;
    region.dstSubresource.mipLevel = 2;
    region.dstOffsets[1].x = extent_.width / 4;
    region.dstOffsets[1].y = extent_.height / 4;
    frame.commandBuffer.blitImage(
        frame.renderImage.get(),
        vk::ImageLayout::eTransferSrcOptimal,
        frame.renderImage.get(),
        vk::ImageLayout::eTransferDstOptimal,
        region,
        vk::Filter::eLinear);
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
    barrier.subresourceRange.baseMipLevel = 2;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barrier);
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.oldLayout = vk::ImageLayout::eUndefined;
    barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.subresourceRange.baseMipLevel = 3;
    frame.commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barrier);
    region.srcSubresource.mipLevel = 2;
    region.srcOffsets[1].x = extent_.width / 4;
    region.srcOffsets[1].y = extent_.height / 4;
    region.dstSubresource.mipLevel = 3;
    region.dstOffsets[1].x = extent_.width / 8;
    region.dstOffsets[1].y = extent_.height / 8;
    frame.commandBuffer.blitImage(
        frame.renderImage.get(),
        vk::ImageLayout::eTransferSrcOptimal,
        frame.renderImage.get(),
        vk::ImageLayout::eTransferDstOptimal,
        region,
        vk::Filter::eLinear);*/
  }

  void SceneView::computeBloomImageArray(Frame &frame) {
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    auto viewport = vk::Viewport{};
    auto scissor = vk::Rect2D{};
    struct {
      float factorR;
      float factorG;
      float factorB;
      float factorA;
      float dx;
      float dy;
    } pushConstants;
    for (auto i = 0; i < 2; ++i) {
      renderPassBegin.framebuffer = frame.largeBloomFramebuffers[i];
      renderPassBegin.renderArea.extent.width =
          frame.largeBloomImageArray.getExtent().width;
      renderPassBegin.renderArea.extent.height =
          frame.largeBloomImageArray.getExtent().height;
      frame.commandBuffer.beginRenderPass(
          renderPassBegin, vk::SubpassContents::eInline);
      frame.commandBuffer.bindPipeline(
          vk::PipelineBindPoint::eGraphics,
          flyweight_->getBlurPipeline(largeBloomKernel_));
      viewport.width = renderPassBegin.renderArea.extent.width;
      viewport.height = renderPassBegin.renderArea.extent.height;
      frame.commandBuffer.setViewport(0, viewport);
      scissor.extent.width = renderPassBegin.renderArea.extent.width;
      scissor.extent.height = renderPassBegin.renderArea.extent.height;
      frame.commandBuffer.setScissor(0, scissor);
      frame.commandBuffer.bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics,
          flyweight_->getBlurPipelineLayout(),
          0,
          frame.largeBlurDescriptorSets[i],
          {});
      pushConstants.factorR = i == 0 ? largeBloomWeight_ : 1.0f;
      pushConstants.factorG = i == 0 ? largeBloomWeight_ : 1.0f;
      pushConstants.factorB = i == 0 ? largeBloomWeight_ : 1.0f;
      pushConstants.factorA = i == 0 ? largeBloomWeight_ : 1.0f;
      pushConstants.dx =
          i == 0 ? 1.0f / frame.largeBloomImageArray.getExtent().width : 0.0f;
      pushConstants.dy =
          i == 0 ? 0.0f : 1.0f / frame.largeBloomImageArray.getExtent().height;
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eFragment,
          0,
          sizeof(pushConstants),
          &pushConstants);
      frame.commandBuffer.draw(3, 1, 0, 0);
      frame.commandBuffer.endRenderPass();
      //
      renderPassBegin.framebuffer = frame.mediumBloomFramebuffers[i];
      renderPassBegin.renderArea.extent.width =
          frame.mediumBloomImageArray.getExtent().width;
      renderPassBegin.renderArea.extent.height =
          frame.mediumBloomImageArray.getExtent().height;
      frame.commandBuffer.beginRenderPass(
          renderPassBegin, vk::SubpassContents::eInline);
      frame.commandBuffer.bindPipeline(
          vk::PipelineBindPoint::eGraphics,
          flyweight_->getBlurPipeline(mediumBloomKernel_));
      viewport.width = renderPassBegin.renderArea.extent.width;
      viewport.height = renderPassBegin.renderArea.extent.height;
      frame.commandBuffer.setViewport(0, viewport);
      scissor.extent.width = renderPassBegin.renderArea.extent.width;
      scissor.extent.height = renderPassBegin.renderArea.extent.height;
      frame.commandBuffer.setScissor(0, scissor);
      frame.commandBuffer.bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics,
          flyweight_->getBlurPipelineLayout(),
          0,
          frame.mediumBlurDescriptorSets[i],
          {});
      pushConstants.factorR = i == 0 ? mediumBloomWeight_ : 1.0f;
      pushConstants.factorG = i == 0 ? mediumBloomWeight_ : 1.0f;
      pushConstants.factorB = i == 0 ? mediumBloomWeight_ : 1.0f;
      pushConstants.factorA = i == 0 ? mediumBloomWeight_ : 1.0f;
      pushConstants.dx =
          i == 0 ? 1.0f / frame.mediumBloomImageArray.getExtent().width : 0.0f;
      pushConstants.dy =
          i == 0 ? 0.0f : 1.0f / frame.mediumBloomImageArray.getExtent().height;
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eFragment,
          0,
          sizeof(pushConstants),
          &pushConstants);
      frame.commandBuffer.draw(3, 1, 0, 0);
      if (i == 1) {
        frame.commandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics, flyweight_->getBloomPipeline());
        frame.commandBuffer.setViewport(0, viewport);
        frame.commandBuffer.setScissor(0, scissor);
        frame.commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            flyweight_->getBloomPipelineLayout(),
            0,
            frame.bloomDescriptorSets[0],
            {});
        frame.commandBuffer.draw(3, 1, 0, 0);
      }
      frame.commandBuffer.endRenderPass();
      //
      renderPassBegin.framebuffer = frame.smallBloomFramebuffers[i];
      renderPassBegin.renderArea.extent.width =
          frame.smallBloomImageArray.getExtent().width;
      renderPassBegin.renderArea.extent.height =
          frame.smallBloomImageArray.getExtent().height;
      frame.commandBuffer.beginRenderPass(
          renderPassBegin, vk::SubpassContents::eInline);
      frame.commandBuffer.bindPipeline(
          vk::PipelineBindPoint::eGraphics,
          flyweight_->getBlurPipeline(smallBloomKernel_));
      viewport.width = renderPassBegin.renderArea.extent.width;
      viewport.height = renderPassBegin.renderArea.extent.height;
      frame.commandBuffer.setViewport(0, viewport);
      scissor.extent.width = renderPassBegin.renderArea.extent.width;
      scissor.extent.height = renderPassBegin.renderArea.extent.height;
      frame.commandBuffer.setScissor(0, scissor);
      frame.commandBuffer.bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics,
          flyweight_->getBlurPipelineLayout(),
          0,
          frame.smallBlurDescriptorSets[i],
          {});
      pushConstants.factorR = i == 0 ? smallBloomWeight_ : 1.0f;
      pushConstants.factorG = i == 0 ? smallBloomWeight_ : 1.0f;
      pushConstants.factorB = i == 0 ? smallBloomWeight_ : 1.0f;
      pushConstants.factorA = i == 0 ? smallBloomWeight_ : 1.0f;
      pushConstants.dx =
          i == 0 ? 1.0f / frame.smallBloomImageArray.getExtent().width : 0.0f;
      pushConstants.dy =
          i == 0 ? 0.0f : 1.0f / frame.smallBloomImageArray.getExtent().height;
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eFragment,
          0,
          sizeof(pushConstants),
          &pushConstants);
      frame.commandBuffer.draw(3, 1, 0, 0);
      if (i == 1) {
        frame.commandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics, flyweight_->getBloomPipeline());
        frame.commandBuffer.setViewport(0, viewport);
        frame.commandBuffer.setScissor(0, scissor);
        frame.commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            flyweight_->getBloomPipelineLayout(),
            0,
            frame.bloomDescriptorSets[1],
            {});
        frame.commandBuffer.draw(3, 1, 0, 0);
      }
      frame.commandBuffer.endRenderPass();
    }

    /* frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(largeBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.largeBlurDescriptorSets[0],
        {});
    {
      auto pushConstants = std::array{0};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 3;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTransfer,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.largeBloomImageArray.getExtent().width + 7) / 8,
        (frame.largeBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(mediumBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.mediumBlurDescriptorSets[0],
        {});
    {
      auto pushConstants = std::array{0};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 2;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTransfer,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.mediumBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.mediumBloomImageArray.getExtent().width + 7) / 8,
        (frame.mediumBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(smallBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.smallBlurDescriptorSets[0],
        {});
    {
      auto pushConstants = std::array{0};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 1;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTransfer,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.smallBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.smallBloomImageArray.getExtent().width + 7) / 8,
        (frame.smallBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipeline(largeBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurPipelineLayout(),
        0,
        frame.largeBlurDescriptorSets[1],
        {});
    {
      auto pushConstants = std::array{1};
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          4,
          pushConstants.data());
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.largeBloomImageArray.getExtent().width + 7) / 8,
        (frame.largeBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipeline(mediumBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipelineLayout(),
        0,
        frame.mediumBlurDescriptorSets[1],
        {});
    {
      struct {
        int blurDirection;
        float blurWeight;
        float blendWeight;
      } pushConstants;
      pushConstants.blurDirection = 1;
      pushConstants.blurWeight = mediumBloomWeight_;
      pushConstants.blendWeight = largeBloomWeight_;
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurBlendPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          12,
          &pushConstants);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.mediumBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.largeBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = {};
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eUndefined;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.mediumBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eTopOfPipe,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.mediumBloomImageArray.getExtent().width + 7) / 8,
        (frame.mediumBloomImageArray.getExtent().height + 7) / 8,
        1);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipeline(smallBloomKernel_));
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBlurBlendPipelineLayout(),
        0,
        frame.smallBlurDescriptorSets[1],
        {});
    {
      struct {
        int blurDirection;
        float blurWeight;
        float blendWeight;
      } pushConstants;
      pushConstants.blurDirection = 1;
      pushConstants.blurWeight = smallBloomWeight_;
      pushConstants.blendWeight = 1.0f;
      frame.commandBuffer.pushConstants(
          flyweight_->getBlurBlendPipelineLayout(),
          vk::ShaderStageFlagBits::eCompute,
          0,
          12,
          &pushConstants);
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        barrier.oldLayout = vk::ImageLayout::eGeneral;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = frame.smallBloomImageArray.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        frame.commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eComputeShader,
            vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        barrier.oldLayout = vk::ImageLayout::eGeneral;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = frame.mediumBloomImageArray.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 1;
        barrier.subresourceRange.layerCount = 1;
        frame.commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eComputeShader,
            vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      {
        auto barrier = vk::ImageMemoryBarrier{};
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
        barrier.oldLayout = vk::ImageLayout::eUndefined;
        barrier.newLayout = vk::ImageLayout::eGeneral;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = frame.smallBloomImageArray.get();
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 1;
        barrier.subresourceRange.layerCount = 1;
        frame.commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eComputeShader,
            {},
            {},
            {},
            barrier);
      }
      frame.commandBuffer.dispatch(
          (frame.smallBloomImageArray.getExtent().width + 7) / 8,
          (frame.smallBloomImageArray.getExtent().height + 7) / 8,
          1);
    }*/
  }

  void SceneView::applyBloom(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getNonDestructiveRenderPass();
    renderPassBegin.framebuffer = frame.primaryFramebuffer;
    renderPassBegin.renderArea.extent.width =
        frame.renderImage.getExtent().width;
    renderPassBegin.renderArea.extent.height =
        frame.renderImage.getExtent().height;
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getBloomPipeline());
    auto viewport = vk::Viewport{};
    viewport.width = renderPassBegin.renderArea.extent.width;
    viewport.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setViewport(0, viewport);
    auto scissor = vk::Rect2D{};
    scissor.extent.width = renderPassBegin.renderArea.extent.width;
    scissor.extent.height = renderPassBegin.renderArea.extent.height;
    frame.commandBuffer.setScissor(0, scissor);
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getBloomPipelineLayout(),
        0,
        frame.bloomDescriptorSets[2],
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();
    /*
    auto &frame = frames_[i];
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eCompute, flyweight_->getBloomPipeline());
    frame.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute,
        flyweight_->getBloomPipelineLayout(),
        0,
        frame.bloomDescriptorSet,
        {});
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.oldLayout = vk::ImageLayout::eGeneral;
      barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.smallBloomImageArray.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 1;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
      barrier.dstAccessMask =
          vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
      barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.newLayout = vk::ImageLayout::eGeneral;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = frame.renderImage.get();
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      frame.commandBuffer.pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader,
          {},
          {},
          {},
          barrier);
    }
    frame.commandBuffer.dispatch(
        (frame.renderImage.getExtent().width + 7) / 8,
        (frame.renderImage.getExtent().height + 7) / 8,
        1);*/
  }

  gsl::not_null<SceneView::Flyweight const *>
  SceneView::getFlyweight() const noexcept {
    return flyweight_;
  }

  gsl::not_null<std::shared_ptr<Scene>> SceneView::getScene() const noexcept {
    return scene_;
  }

  void
  SceneView::setScene(gsl::not_null<std::shared_ptr<Scene>> scene) noexcept {
    scene_ = std::move(scene);
  }

  Extent2u const &SceneView::getExtent() const noexcept {
    return extent_;
  }

  void SceneView::setExtent(Extent2u const &extent) noexcept {
    extent_ = extent;
  }

  GpuBuffer const &SceneView::getUniformBuffer() const noexcept {
    return uniformBuffer_;
  }

  GpuImage const &SceneView::getSkyViewImage() const noexcept {
    return skyViewImage_;
  }

  GpuImage const &SceneView::getRenderImage(std::size_t i) const noexcept {
    return frames_[i].renderImage;
  }

  vk::ImageView SceneView::getSkyViewImageView(std::size_t i) const noexcept {
    return frames_[i].skyViewImageView;
  }

  vk::ImageView
  SceneView::getFullRenderImageView(std::size_t i) const noexcept {
    return frames_[i].renderImageView;
  }

  vk::Semaphore SceneView::getSemaphore(std::size_t i) const noexcept {
    return frames_[i].semaphore;
  }

  Eigen::Matrix4f const &SceneView::getViewMatrix() const noexcept {
    return viewMatrix_;
  }

  void SceneView::setViewMatrix(Eigen::Matrix4f const &m) noexcept {
    viewMatrix_ = m;
    invViewMatrix_ = m.inverse();
  }

  Eigen::Matrix4f const &SceneView::getProjectionMatrix() const noexcept {
    return projectionMatrix_;
  }

  void SceneView::setProjectionMatrix(Eigen::Matrix4f const &m) noexcept {
    projectionMatrix_ = m;
    invProjectionMatrix_ = m.inverse();
  }

  float SceneView::getExposure() const noexcept {
    return exposure_;
  }

  void SceneView::setExposure(float exposure) noexcept {
    exposure_ = exposure;
  }
} // namespace imp