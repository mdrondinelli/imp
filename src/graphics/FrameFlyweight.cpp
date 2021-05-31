#include "FrameFlyweight.h"

#include <filesystem>
#include <fstream>

namespace imp {
  FrameFlyweight::FrameFlyweight(
      Window &window, FrameFlyweightCreateInfo const &createInfo):
      window_{&window},
      transmittanceLutFlyweight_{createInfo.transmittanceLutFlyweight},
      skyViewLutFlyweight_{createInfo.skyViewLutFlyweight},
      renderPass_{createRenderPass()},
      pipelineLayout_{createPipelineLayout()},
      pipeline_{createPipeline()} {}

  Window *FrameFlyweight::getWindow() const noexcept {
    return window_;
  }

  std::shared_ptr<TransmittanceLut::Flyweight const>
  FrameFlyweight::getTransmittanceLutFlyweight() const noexcept {
    return transmittanceLutFlyweight_;
  }

  std::shared_ptr<SkyViewLut::Flyweight const>
  FrameFlyweight::getSkyViewLutFlyweight() const noexcept {
    return skyViewLutFlyweight_;
  }

  vk::RenderPass FrameFlyweight::getRenderPass() const noexcept {
    return *renderPass_;
  }

  vk::PipelineLayout FrameFlyweight::getPipelineLayout() const noexcept {
    return *pipelineLayout_;
  }

  vk::Pipeline FrameFlyweight::getPipeline() const noexcept {
    return *pipeline_;
  }

  vk::UniqueRenderPass FrameFlyweight::createRenderPass() {
    auto attachment = vk::AttachmentDescription{};
    attachment.format = window_->getSurfaceFormat().format;
    attachment.samples = vk::SampleCountFlagBits::e1;
    attachment.loadOp = vk::AttachmentLoadOp::eClear;
    attachment.storeOp = vk::AttachmentStoreOp::eStore;
    attachment.initialLayout = vk::ImageLayout::eUndefined;
    attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    auto subpassColorAttachment = vk::AttachmentReference{};
    subpassColorAttachment.attachment = 0;
    subpassColorAttachment.layout = vk::ImageLayout::eColorAttachmentOptimal;
    auto subpass = vk::SubpassDescription{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &subpassColorAttachment;
    auto createInfo = vk::RenderPassCreateInfo{};
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    return window_->getContext()->getDevice().createRenderPassUnique(
        createInfo);
  }

  vk::UniquePipelineLayout FrameFlyweight::createPipelineLayout() {
    auto setLayouts = std::array{
        transmittanceLutFlyweight_->getTextureDescriptorSetLayout(),
        skyViewLutFlyweight_->getTextureDescriptorSetLayout()};
    auto pushConstantRange = vk::PushConstantRange{};
    pushConstantRange.stageFlags =
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 84;
    auto createInfo = vk::PipelineLayoutCreateInfo{};
    createInfo.setLayoutCount = static_cast<std::uint32_t>(setLayouts.size());
    createInfo.pSetLayouts = setLayouts.data();
    createInfo.pushConstantRangeCount = 1;
    createInfo.pPushConstantRanges = &pushConstantRange;
    return window_->getContext()->getDevice().createPipelineLayoutUnique(
        createInfo);
  }

  namespace {
    vk::UniqueShaderModule
    createShaderModule(GpuContext &context, std::filesystem::path const &path) {
      auto code = std::vector<char>{};
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open(path, std::ios::binary);
      in.seekg(0, std::ios::end);
      auto size = static_cast<std::size_t>(in.tellg());
      if (size % 4 != 0) {
        throw std::runtime_error{"failed to create shader module"};
      }
      code.resize(size);
      in.seekg(0, std::ios::beg);
      in.read(code.data(), code.size());
      auto moduleCreateInfo = vk::ShaderModuleCreateInfo{};
      moduleCreateInfo.codeSize = static_cast<std::uint32_t>(code.size());
      moduleCreateInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
      return context.getDevice().createShaderModuleUnique(moduleCreateInfo);
    }
  } // namespace

  vk::UniquePipeline FrameFlyweight::createPipeline() {
    auto &context = *window_->getContext();
    auto vertModule = createShaderModule(context, "./res/AtmosphereVert.spv");
    auto vertStage = vk::PipelineShaderStageCreateInfo{};
    vertStage.stage = vk::ShaderStageFlagBits::eVertex;
    vertStage.module = *vertModule;
    vertStage.pName = "main";
    auto fragModule = createShaderModule(context, "./res/AtmosphereFrag.spv");
    auto fragStage = vk::PipelineShaderStageCreateInfo{};
    fragStage.stage = vk::ShaderStageFlagBits::eFragment;
    fragStage.module = *fragModule;
    fragStage.pName = "main";
    auto stages = std::array{vertStage, fragStage};
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    rasterizationState.polygonMode = vk::PolygonMode::eFill;
    rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
    rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationState.lineWidth = 1.0f;
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    // auto depth_stencil_state = vk::PipelineDepthStencilStateCreateInfo{};
    // depth_stencil_state.depthTestEnable = false;
    // depth_stencil_state.depthCompareOp = vk::CompareOp::eEqual;
    // depth_stencil_state.minDepthBounds = 0.0f;
    // depth_stencil_state.maxDepthBounds = 1.0f;
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
    // info.pDepthStencilState = &depth_stencil_state;
    createInfo.pColorBlendState = &colorBlendState;
    createInfo.pDynamicState = &dynamicState;
    createInfo.layout = *pipelineLayout_;
    createInfo.renderPass = *renderPass_;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = vk::Pipeline{};
    createInfo.basePipelineIndex = -1;
    return context.getDevice()
        .createGraphicsPipelineUnique({}, createInfo)
        .value;
  }
} // namespace imp