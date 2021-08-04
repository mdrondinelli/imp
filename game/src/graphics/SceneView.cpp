#include "SceneView.h"

#include <fstream>
#include <iostream>

#include "../system/GpuContext.h"
#include "../util/Align.h"
#include "../util/Math.h"
#include "Scene.h"

namespace imp {
  using Eigen::Matrix4f;
  using Eigen::Vector3f;
  using Eigen::Vector4f;

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
      primaryPipelines_{createPrimaryPipelines()},
      identityPipeline_{createIdentityPipeline()},
      blurPipelines_{createBlurPipelines()},
      bloomPipeline_{createBloomPipeline()},
      generalSampler_{createGeneralSampler()},
      skyViewSampler_{createSkyViewSampler()} {}

  vk::RenderPass SceneView::Flyweight::createRenderPass() const {
    auto attachmentDesc = GpuAttachmentDescription{};
    attachmentDesc.format = vk::Format::eR16G16B16A16Sfloat;
    attachmentDesc.samples = vk::SampleCountFlagBits::e1;
    attachmentDesc.loadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDesc.initialLayout = vk::ImageLayout::eUndefined;
    attachmentDesc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto attachmentRef = GpuAttachmentReference{};
    attachmentRef.attachment = 0;
    attachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = GpuSubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachments = {&attachmentRef, 1};
    auto dependencies = std::array<GpuSubpassDependency, 2>{};
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
    auto createInfo = GpuRenderPassCreateInfo{};
    createInfo.attachments = {&attachmentDesc, 1};
    createInfo.subpasses = {&subpass, 1};
    createInfo.dependencies = dependencies;
    return context_->createRenderPass(createInfo);
  }

  vk::RenderPass SceneView::Flyweight::createNonDestructiveRenderPass() const {
    auto attachmentDesc = GpuAttachmentDescription{};
    attachmentDesc.format = vk::Format::eR16G16B16A16Sfloat;
    attachmentDesc.samples = vk::SampleCountFlagBits::e1;
    attachmentDesc.loadOp = vk::AttachmentLoadOp::eLoad;
    attachmentDesc.storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDesc.initialLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    attachmentDesc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto attachmentRef = GpuAttachmentReference{};
    attachmentRef.attachment = 0;
    attachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = GpuSubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachments = {&attachmentRef, 1};
    auto dependencies = std::array<GpuSubpassDependency, 2>{};
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
    auto createInfo = GpuRenderPassCreateInfo{};
    createInfo.attachments = {&attachmentDesc, 1};
    createInfo.subpasses = {&subpass, 1};
    createInfo.dependencies = dependencies;
    return context_->createRenderPass(createInfo);
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

  std::unordered_map<bool, vk::Pipeline>
  SceneView::Flyweight::createPrimaryPipelines() const {
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
    auto pipelines = std::unordered_map<bool, vk::Pipeline>{};
    for (auto kernelSize : std::vector<std::int32_t>{0, 1}) {
      specializationInfo.pData = &kernelSize;
      pipelines.emplace(
          kernelSize,
          context_->getDevice().createGraphicsPipeline({}, createInfo).value);
    }
    return pipelines;
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

  std::unordered_map<int, vk::Pipeline>
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
    auto pipelines = std::unordered_map<int, vk::Pipeline>{};
    for (auto kernelSize : std::vector<std::int32_t>{
             3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33}) {
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
    createInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    createInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    createInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
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
    for (auto [_, pipeline] : primaryPipelines_) {
      device.destroy(pipeline);
    }
    device.destroy(skyViewPipeline_);
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

  vk::Pipeline SceneView::Flyweight::getPrimaryPipeline(
      bool antiAliasingEnabled) const noexcept {
    return primaryPipelines_.at(antiAliasingEnabled);
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
      GpuImage &&skyViewImage,
      GpuImage &&renderImage,
      std::vector<GpuImage> &&bloomImages):
      skyViewImage{std::move(skyViewImage)},
      primaryImage{std::move(renderImage)},
      bloomImages{std::move(bloomImages)} {}

  SceneView::SceneView(
      gsl::not_null<Flyweight const *> flyweight,
      gsl::not_null<std::shared_ptr<Scene>> scene,
      Extent2u const &extent) noexcept:
      flyweight_{flyweight},
      scene_{std::move(scene)},
      extent_{extent},
      descriptorPool_{createDescriptorPool()},
      uniformBuffer_{createUniformBuffer()},
      frames_{createFrames()},
      viewMatrix_{Matrix4f::Identity()},
      projectionMatrix_{Matrix4f::Identity()},
      prevViewMatrix_{Matrix4f::Identity()},
      prevProjectionMatrix_{Matrix4f::Identity()},
      exposure_{1.0f},
      antiAliasingEnabled_{false},
      antiAliasingAlpha_{0.25f},
      antiAliasingJitter_{0.0f, 0.0f},
      bloomEnabled_{false},
      bloomBlurCounts_{1, 2, 3, 4},
      bloomBlurSizes_{17, 23, 27, 33},
      bloomSpectra_{{0.0001f}, {0.00015f}, {0.0003f}, {0.0009f}},
      firstFrame_{true} {
    for (auto i = std::size_t{}; i < frames_.size(); ++i) {
      auto &frame = frames_[i];
      initSkyViewImageView(frame);
      initPrimaryImageViews(frame);
      initBloomImageViews(frame);
      initSkyViewFramebuffer(frame);
      initPrimaryFramebuffers(frame);
      initBloomFramebuffers(frame);
      allocateDescriptorSets(i);
      initSkyViewDescriptorSet(i);
      initPrimaryDescriptorSet(i);
      initPrimaryImageDescriptorSets(frame);
      initBloomTextureDescriptorSets(frame);
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
        // primary texture
        {vk::DescriptorType::eCombinedImageSampler, 5 * frameCount32},
        // bloom texture
        {vk::DescriptorType::eCombinedImageSampler, 8 * frameCount32}};
    auto createInfo = vk::DescriptorPoolCreateInfo{};
    createInfo.maxSets = 15 * frameCount32;
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

  std::vector<SceneView::Frame> SceneView::createFrames() const {
    auto frames = std::vector<SceneView::Frame>{};
    frames.reserve(flyweight_->getFrameCount());
    for (auto i = std::size_t{}; i < frames.capacity(); ++i) {
      frames.emplace_back(
          createSkyViewImage(), createPrimaryImage(), createBloomImages());
    }
    return frames;
  }

  GpuImage SceneView::createSkyViewImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = SKY_VIEW_IMAGE_EXTENT;
    image.mipLevels = 1;
    image.arrayLayers = 1;
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

  GpuImage SceneView::createPrimaryImage() const {
    auto image = vk::ImageCreateInfo{};
    image.imageType = vk::ImageType::e2D;
    image.format = vk::Format::eR16G16B16A16Sfloat;
    image.extent = vk::Extent3D{extent_.width, extent_.height, 1};
    image.mipLevels = 5;
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

  std::vector<GpuImage> SceneView::createBloomImages() const {
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
    auto images = std::vector<GpuImage>{};
    for (auto i = 0; i < 4; ++i) {
      images.emplace_back(
          flyweight_->getContext()->getAllocator(), image, allocation);
      image.extent.width /= 2;
      image.extent.height /= 2;
    }
    return images;
  }

  void SceneView::initSkyViewImageView(Frame &frame) const {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.skyViewImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.skyViewImage.getFormat();
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frame.skyViewImageView =
        flyweight_->getContext()->getDevice().createImageView(createInfo);
  }

  void SceneView::initPrimaryImageViews(Frame &frame) const {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.image = frame.primaryImage.get();
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = frame.primaryImage.getFormat();
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    frame.primaryImageViews.clear();
    for (auto i = 0; i < 5; ++i) {
      createInfo.subresourceRange.baseMipLevel = i;
      frame.primaryImageViews.emplace_back(
          flyweight_->getContext()->getDevice().createImageView(createInfo));
    }
  }

  void SceneView::initBloomImageViews(Frame &frame) const {
    auto createInfo = vk::ImageViewCreateInfo{};
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.layerCount = 1;
    frame.bloomImageViews.clear();
    for (auto i = 0; i < 4; ++i) {
      createInfo.image = frame.bloomImages[i].get();
      createInfo.format = frame.bloomImages[i].getFormat();
      for (auto j = 0; j < 2; ++j) {
        createInfo.subresourceRange.baseArrayLayer = j;
        frame.bloomImageViews.emplace_back(
            flyweight_->getContext()->getDevice().createImageView(createInfo));
      }
    }
  }

  void SceneView::initSkyViewFramebuffer(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &frame.skyViewImageView;
    createInfo.width = frame.skyViewImage.getExtent().width;
    createInfo.height = frame.skyViewImage.getExtent().height;
    createInfo.layers = 1;
    frame.skyViewFramebuffer =
        flyweight_->getContext()->getDevice().createFramebuffer(createInfo);
  }

  void SceneView::initPrimaryFramebuffers(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.width = frame.primaryImage.getExtent().width;
    createInfo.height = frame.primaryImage.getExtent().height;
    createInfo.layers = 1;
    frame.primaryFramebuffers.clear();
    for (auto i = 0; i < 5; ++i) {
      createInfo.pAttachments = &frame.primaryImageViews[i];
      frame.primaryFramebuffers.emplace_back(
          flyweight_->getContext()->getDevice().createFramebuffer(createInfo));
      createInfo.width /= 2;
      createInfo.height /= 2;
    }
  }

  void SceneView::initBloomFramebuffers(Frame &frame) const {
    auto createInfo = vk::FramebufferCreateInfo{};
    createInfo.renderPass = flyweight_->getRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.width = frame.primaryImage.getExtent().width / 2;
    createInfo.height = frame.primaryImage.getExtent().height / 2;
    createInfo.layers = 1;
    frame.bloomFramebuffers.clear();
    for (auto i = 0; i < 4; ++i) {
      for (auto j = 0; j < 2; ++j) {
        createInfo.pAttachments = &frame.bloomImageViews[2 * i + j];
        frame.bloomFramebuffers.emplace_back(
            flyweight_->getContext()->getDevice().createFramebuffer(
                createInfo));
      }
      createInfo.width /= 2;
      createInfo.height /= 2;
    }
  }

  void SceneView::allocateDescriptorSets(std::size_t i) {
    auto &frame = frames_[i];
    auto device = flyweight_->getContext()->getDevice();
    auto skyViewSetLayout = flyweight_->getSkyViewDescriptorSetLayout();
    auto primarySetLayout = flyweight_->getPrimaryDescriptorSetLayout();
    auto postProcessSetLayout = flyweight_->getIdentityDescriptorSetLayout();
    auto allocateInfo = vk::DescriptorSetAllocateInfo{};
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &skyViewSetLayout;
    device.allocateDescriptorSets(&allocateInfo, &frame.skyViewDescriptorSet);
    allocateInfo.pSetLayouts = &primarySetLayout;
    device.allocateDescriptorSets(&allocateInfo, &frame.primaryDescriptorSet);
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &postProcessSetLayout;
    frame.primaryTextureDescriptorSets.resize(
        frame.primaryImage.getMipLevels());
    for (auto &set : frame.primaryTextureDescriptorSets) {
      device.allocateDescriptorSets(&allocateInfo, &set);
    }
    frame.bloomTextureDescriptorSets.resize(
        2 * frame.primaryImage.getMipLevels() - 2);
    for (auto &set : frame.bloomTextureDescriptorSets) {
      device.allocateDescriptorSets(&allocateInfo, &set);
    }
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

  void SceneView::initPrimaryImageDescriptorSets(Frame &frame) const {
    auto info = vk::DescriptorImageInfo{};
    info.sampler = flyweight_->getGeneralSampler();
    info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto write = vk::WriteDescriptorSet{};
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.pImageInfo = &info;
    for (auto i = 0; i < 5; ++i) {
      info.imageView = frame.primaryImageViews[i];
      write.dstSet = frame.primaryTextureDescriptorSets[i];
      flyweight_->getContext()->getDevice().updateDescriptorSets(write, {});
    }
  }

  void SceneView::initBloomTextureDescriptorSets(Frame &frame) const {
    auto info = vk::DescriptorImageInfo{};
    info.sampler = flyweight_->getGeneralSampler();
    info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    auto write = vk::WriteDescriptorSet{};
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    write.pImageInfo = &info;
    for (auto i = 0; i < 4 * 2; ++i) {
      info.imageView = frame.bloomImageViews[i];
      write.dstSet = frame.bloomTextureDescriptorSets[i];
      flyweight_->getContext()->getDevice().updateDescriptorSets(write, 0);
    }
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
      for (auto framebuffer : frame.bloomFramebuffers) {
        device.destroy(framebuffer);
      }
      for (auto framebuffer : frame.primaryFramebuffers) {
        device.destroy(framebuffer);
      }
      device.destroy(frame.skyViewFramebuffer);
      for (auto imageView : frame.bloomImageViews) {
        device.destroy(imageView);
      }
      for (auto imageView : frame.primaryImageViews) {
        device.destroy(imageView);
      }
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
    if (frame.primaryImage.getExtent() !=
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
    prevViewMatrix_ = viewMatrix_;
    prevProjectionMatrix_ = projectionMatrix_;
    antiAliasingJitter_ = nextLds(antiAliasingJitter_);
    firstFrame_ = false;
  }

  void SceneView::updateRenderImages(std::size_t i) {
    auto device = flyweight_->getContext()->getDevice();
    auto &frame = frames_[i];
    for (auto framebuffer : frame.bloomFramebuffers) {
      device.destroy(framebuffer);
    }
    for (auto framebuffer : frame.primaryFramebuffers) {
      device.destroy(framebuffer);
    }
    for (auto imageView : frame.bloomImageViews) {
      device.destroy(imageView);
    }
    for (auto imageView : frame.primaryImageViews) {
      device.destroy(imageView);
    }
    frame.primaryImage = createPrimaryImage();
    frame.bloomImages = createBloomImages();
    initPrimaryImageViews(frame);
    initBloomImageViews(frame);
    initPrimaryFramebuffers(frame);
    initBloomFramebuffers(frame);
    initPrimaryImageDescriptorSets(frame);
    initBloomTextureDescriptorSets(frame);
  }

  void SceneView::updateUniformBuffer(std::size_t frameIndex) {
    Matrix4f invViewMatrix = viewMatrix_.inverse();
    Matrix4f invProjectionMatrix = projectionMatrix_.inverse();
    Matrix4f invJitterMatrix = Matrix4f::Identity();
    invJitterMatrix(0, 3) =
        (2.0f * antiAliasingJitter_(0) - 1.0f) / extent_.width;
    invJitterMatrix(1, 3) =
        (2.0f * antiAliasingJitter_(1) - 1.0f) / extent_.height;
    Vector3f position = invViewMatrix.col(3).head<3>();
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
    Matrix4f skyViewDirectionMatrix =
        skyViewMatrix * invViewMatrix * invProjectionMatrix * invJitterMatrix;
    Matrix4f antiAliasingPositionMatrix = prevProjectionMatrix_ *
                                          prevViewMatrix_ * invViewMatrix *
                                          invProjectionMatrix;
    auto clipPositions = std::array{
        Vector4f{-1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{-1.0f, -1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, -1.0f, 1.0f, 1.0f}};
    auto skyViewDirections = std::array<Vector4f, 4>{};
    auto antiAliasingPositions = std::array<Vector4f, 4>{};
    for (auto i = 0; i < 4; ++i) {
      skyViewDirections[i] = skyViewDirectionMatrix * clipPositions[i];
      skyViewDirections[i] /= skyViewDirections[i].w();
      skyViewDirections[i].w() = 0.0f;
      skyViewDirections[i].normalize();
      antiAliasingPositions[i] = antiAliasingPositionMatrix * clipPositions[i];
      antiAliasingPositions[i] /= antiAliasingPositions[i].w();
      antiAliasingPositions[i].z() = 0.0f;
      antiAliasingPositions[i].w() = 0.0f;
    }
    Vector3f skyViewSunDirection = skyViewMatrix.topLeftCorner<3, 3>() *
                                   scene_->getSunLight()->getDirection();
    auto offset = UNIFORM_BUFFER_STRIDE * frameIndex;
    auto data = uniformBuffer_.getMappedData() + offset;
    std::memcpy(data + 0, &skyViewDirections[0], 12);
    std::memcpy(data + 16, &skyViewDirections[1], 12);
    std::memcpy(data + 32, &skyViewDirections[2], 12);
    std::memcpy(data + 48, &skyViewDirections[3], 12);
    std::memcpy(data + 64, &skyViewSunDirection, 12);
    std::memcpy(data + 80, &antiAliasingPositions[0], 8);
    std::memcpy(data + 96, &antiAliasingPositions[1], 8);
    std::memcpy(data + 112, &antiAliasingPositions[2], 8);
    std::memcpy(data + 128, &antiAliasingPositions[3], 8);
    std::memcpy(data + 144, &antiAliasingAlpha_, 4);
    std::memcpy(data + 148, &altitude, 4);
    std::memcpy(data + 152, &exposure_, 4);
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
    if (bloomEnabled_) {
      renderBloom(frame);
      applyBloom(i);
    }
    frame.commandBuffer.end();
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.commandBuffer;
    flyweight_->getContext()->getGraphicsQueue().submit(submitInfo);
  }

  void SceneView::computeSkyViewImage(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.framebuffer = frame.skyViewFramebuffer;
    renderPassBegin.renderArea.extent.width =
        frame.skyViewImage.getExtent().width;
    renderPassBegin.renderArea.extent.height =
        frame.skyViewImage.getExtent().height;
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
  }

  void SceneView::computeRenderImage(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.framebuffer = frame.primaryFramebuffers[0];
    renderPassBegin.renderArea.extent.width =
        frame.primaryImage.getExtent().width;
    renderPassBegin.renderArea.extent.height =
        frame.primaryImage.getExtent().height;
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    frame.commandBuffer.beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    frame.commandBuffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getPrimaryPipeline(!firstFrame_ && antiAliasingEnabled_));
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
  }

  void SceneView::computeRenderImageMips(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.renderArea.extent.width =
        frame.primaryImage.getExtent().width;
    renderPassBegin.renderArea.extent.height =
        frame.primaryImage.getExtent().height;
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    auto viewport = vk::Viewport{};
    auto scissor = vk::Rect2D{};
    for (auto i = 0; i < 4; ++i) {
      renderPassBegin.framebuffer = frame.primaryFramebuffers[i + 1];
      renderPassBegin.renderArea.extent.width /= 2;
      renderPassBegin.renderArea.extent.height /= 2;
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
          frame.primaryTextureDescriptorSets[i],
          {});
      frame.commandBuffer.draw(3, 1, 0, 0);
      frame.commandBuffer.endRenderPass();
    }
  }

  void SceneView::renderBloom(Frame &frame) const {
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
    pushConstants.factorA = 1.0f;
    for (auto i = 3; i >= 0; --i) {
      auto width = frame.bloomImages[i].getExtent().width;
      auto height = frame.bloomImages[i].getExtent().height;
      renderPassBegin.renderArea.extent.width = width;
      renderPassBegin.renderArea.extent.height = height;
      viewport.width = width;
      viewport.height = height;
      scissor.extent.width = width;
      scissor.extent.height = height;
      for (auto j = 0; j < bloomBlurCounts_[i]; ++j) {
        renderPassBegin.framebuffer = frame.bloomFramebuffers[2 * i];
        frame.commandBuffer.beginRenderPass(
            renderPassBegin, vk::SubpassContents::eInline);
        frame.commandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            flyweight_->getBlurPipeline(bloomBlurSizes_[i]));
        frame.commandBuffer.setViewport(0, viewport);
        frame.commandBuffer.setScissor(0, scissor);
        frame.commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            flyweight_->getBlurPipelineLayout(),
            0,
            j != 0 ? frame.bloomTextureDescriptorSets[2 * i + 1]
                   : frame.primaryTextureDescriptorSets[i + 1],
            {});
        pushConstants.factorR = j != 0 ? 1.0f : bloomSpectra_[i].r();
        pushConstants.factorG = j != 0 ? 1.0f : bloomSpectra_[i].g();
        pushConstants.factorB = j != 0 ? 1.0f : bloomSpectra_[i].b();
        pushConstants.dx = 1.0f / width;
        pushConstants.dy = 0.0f;
        frame.commandBuffer.pushConstants(
            flyweight_->getBlurPipelineLayout(),
            vk::ShaderStageFlagBits::eFragment,
            0,
            sizeof(pushConstants),
            &pushConstants);
        frame.commandBuffer.draw(3, 1, 0, 0);
        frame.commandBuffer.endRenderPass();
        renderPassBegin.framebuffer = frame.bloomFramebuffers[2 * i + 1];
        frame.commandBuffer.beginRenderPass(
            renderPassBegin, vk::SubpassContents::eInline);
        frame.commandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            flyweight_->getBlurPipeline(bloomBlurSizes_[i]));
        frame.commandBuffer.setViewport(0, viewport);
        frame.commandBuffer.setScissor(0, scissor);
        frame.commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            flyweight_->getBlurPipelineLayout(),
            0,
            frame.bloomTextureDescriptorSets[2 * i],
            {});
        pushConstants.factorR = 1.0f;
        pushConstants.factorG = 1.0f;
        pushConstants.factorB = 1.0f;
        pushConstants.dx = 0.0f;
        pushConstants.dy = 1.0f / height;
        frame.commandBuffer.pushConstants(
            flyweight_->getBlurPipelineLayout(),
            vk::ShaderStageFlagBits::eFragment,
            0,
            sizeof(pushConstants),
            &pushConstants);
        frame.commandBuffer.draw(3, 1, 0, 0);
        if (i != 3 && j + 1 == bloomBlurCounts_[i]) {
          frame.commandBuffer.bindPipeline(
              vk::PipelineBindPoint::eGraphics, flyweight_->getBloomPipeline());
          frame.commandBuffer.setViewport(0, viewport);
          frame.commandBuffer.setScissor(0, scissor);
          frame.commandBuffer.bindDescriptorSets(
              vk::PipelineBindPoint::eGraphics,
              flyweight_->getBloomPipelineLayout(),
              0,
              frame.bloomTextureDescriptorSets[2 * i + 3],
              {});
          frame.commandBuffer.draw(3, 1, 0, 0);
        }
        frame.commandBuffer.endRenderPass();
      }
    }
  }

  void SceneView::applyBloom(std::size_t i) {
    auto &frame = frames_[i];
    auto clearValue = vk::ClearValue{};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getNonDestructiveRenderPass();
    renderPassBegin.framebuffer = frame.primaryFramebuffers[0];
    renderPassBegin.renderArea.extent.width =
        frame.primaryImage.getExtent().width;
    renderPassBegin.renderArea.extent.height =
        frame.primaryImage.getExtent().height;
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
        frame.bloomTextureDescriptorSets[1],
        {});
    frame.commandBuffer.draw(3, 1, 0, 0);
    frame.commandBuffer.endRenderPass();
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

  GpuImage const &SceneView::getSkyViewImage(std::size_t i) const noexcept {
    return frames_[i].skyViewImage;
  }

  GpuImage const &SceneView::getRenderImage(std::size_t i) const noexcept {
    return frames_[i].primaryImage;
  }

  vk::ImageView SceneView::getSkyViewImageView(std::size_t i) const noexcept {
    return frames_[i].skyViewImageView;
  }

  vk::ImageView
  SceneView::getFullRenderImageView(std::size_t i) const noexcept {
    return frames_[i].primaryImageViews[0];
  }

  vk::Semaphore SceneView::getSemaphore(std::size_t i) const noexcept {
    return frames_[i].semaphore;
  }

  Eigen::Matrix4f const &SceneView::getViewMatrix() const noexcept {
    return viewMatrix_;
  }

  void SceneView::setViewMatrix(Eigen::Matrix4f const &m) noexcept {
    viewMatrix_ = m;
  }

  Eigen::Matrix4f const &SceneView::getProjectionMatrix() const noexcept {
    return projectionMatrix_;
  }

  void SceneView::setProjectionMatrix(Eigen::Matrix4f const &m) noexcept {
    projectionMatrix_ = m;
  }

  float SceneView::getExposure() const noexcept {
    return exposure_;
  }

  void SceneView::setExposure(float exposure) noexcept {
    exposure_ = exposure;
  }

  bool SceneView::isAntiAliasingEnabled() const noexcept {
    return antiAliasingEnabled_;
  }

  void SceneView::setAntiAliasingEnabled(bool antiAliasingEnabled) noexcept {
    antiAliasingEnabled_ = antiAliasingEnabled;
  }

  bool SceneView::isBloomEnabled() const noexcept {
    return bloomEnabled_;
  }

  void SceneView::setBloomEnabled(bool bloomEnabled) noexcept {
    bloomEnabled_ = bloomEnabled;
  }

  unsigned SceneView::getBloomBlurCount(unsigned level) const noexcept {
    gsl_Expects(level < 4);
    return bloomBlurCounts_[level];
  }

  void
  SceneView::setBloomBlurCount(unsigned level, unsigned blurCount) noexcept {
    gsl_Expects(level < 4);
    bloomBlurCounts_[level] = blurCount;
  }

  unsigned SceneView::getBloomBlurSize(unsigned level) const noexcept {
    gsl_Expects(level < 4);
    return bloomBlurSizes_[level];
  }

  void SceneView::setBloomBlurSize(unsigned level, unsigned blurSize) noexcept {
    gsl_Expects(level < 4);
    bloomBlurSizes_[level] = blurSize;
  }

  Spectrum const &SceneView::getBloomSpectrum(unsigned level) const noexcept {
    gsl_Expects(level < 4);
    return bloomSpectra_[level];
  }

  void SceneView::setBloomSpectrum(
      unsigned level, Spectrum const &spectrum) noexcept {
    gsl_Expects(level < 4);
    bloomSpectra_[level] = spectrum;
  }
} // namespace imp