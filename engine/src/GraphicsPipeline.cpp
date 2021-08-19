// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.engine.gpu;
import <cstring>;
import <array>;
import <span>;
import <variant>;
import <vector>;
// clang-format on

static_assert(sizeof(vk::Bool32) == 4);
static_assert(sizeof(float) == 4);
static_assert(sizeof(std::int32_t) == 4);
static_assert(sizeof(std::uint32_t) == 4);

namespace mobula {
  GraphicsPipeline::GraphicsPipeline(
      vk::Device device,
      ShaderModuleCache &shaderModuleCache,
      GraphicsPipelineParams const &params):
      params_{params} {
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    auto stages = std::vector<vk::PipelineShaderStageCreateInfo>{};
    stages.reserve(
        1 + 2 * params.tessellation.has_value() +
        params.geometryStage.has_value() +
        params.rasterization.has_value());
    auto vertexStageSpecializationInfo = vk::SpecializationInfo{};
    auto vertexStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto vertexStageSpecializationData = std::vector<std::byte>{};
    copyPipelineShaderStageParams(
        stages.emplace_back(),
        vertexStageSpecializationInfo,
        vertexStageSpecializationMapEntries,
        vertexStageSpecializationData,
        shaderModuleCache,
        vk::ShaderStageFlagBits::eVertex,
        params.vertexStage);
    auto tessellationControlStageSpecializationInfo = vk::SpecializationInfo{};
    auto tessellationControlStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto tessellationControlStageSpecializationData = std::vector<std::byte>{};
    auto tessellationEvaluationStageSpecializationInfo =
        vk::SpecializationInfo{};
    auto tessellationEvaluationStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto tessellationEvaluationStageSpecializationData =
        std::vector<std::byte>{};
    if (params.tessellation) {
      copyPipelineShaderStageParams(
          stages.emplace_back(),
          tessellationControlStageSpecializationInfo,
          tessellationControlStageSpecializationMapEntries,
          tessellationControlStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eTessellationControl,
          params.tessellation->controlStage);
      copyPipelineShaderStageParams(
          stages.emplace_back(),
          tessellationEvaluationStageSpecializationInfo,
          tessellationEvaluationStageSpecializationMapEntries,
          tessellationEvaluationStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eTessellationEvaluation,
          params.tessellation->evaluationStage);
    }
    auto geometryStageSpecializationInfo = vk::SpecializationInfo{};
    auto geometryStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto geometryStageSpecializationData = std::vector<std::byte>{};
    if (params.geometryStage) {
      copyPipelineShaderStageParams(
          stages.emplace_back(),
          geometryStageSpecializationInfo,
          geometryStageSpecializationMapEntries,
          geometryStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eGeometry,
          *params.geometryStage);
    }
    auto fragmentStageSpecializationInfo = vk::SpecializationInfo{};
    auto fragmentStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto fragmentStageSpecializationData = std::vector<std::byte>{};
    if (params.rasterization) {
      copyPipelineShaderStageParams(
          stages.emplace_back(),
          fragmentStageSpecializationInfo,
          fragmentStageSpecializationMapEntries,
          fragmentStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eFragment,
          params.rasterization->fragmentStage);
    }
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto vertexBindingDescriptions =
        std::vector<vk::VertexInputBindingDescription>{};
    vertexBindingDescriptions.reserve(
        params.inputAssembly.vertexBindings.size());
    for (auto i = std::uint32_t{};
         i < params.inputAssembly.vertexBindings.size();
         ++i) {
      auto &binding = params.inputAssembly.vertexBindings[i];
      vertexBindingDescriptions.emplace_back(
          i, binding.stride, binding.inputRate);
    }
    vertexInputState.vertexBindingDescriptionCount =
        static_cast<std::uint32_t>(vertexBindingDescriptions.size());
    vertexInputState.pVertexBindingDescriptions =
        vertexBindingDescriptions.data();
    auto vertexAttributeDescriptions =
        std::vector<vk::VertexInputAttributeDescription>{};
    vertexAttributeDescriptions.reserve(
        params.inputAssembly.vertexAttributes.size());
    for (auto i = std::uint32_t{};
         i < params.inputAssembly.vertexAttributes.size();
         ++i) {
      auto &attribute = params.inputAssembly.vertexAttributes[i];
      vertexAttributeDescriptions.emplace_back(
          i, attribute.binding, attribute.format, attribute.offset);
    }
    vertexInputState.vertexAttributeDescriptionCount =
        static_cast<std::uint32_t>(vertexAttributeDescriptions.size());
    vertexInputState.pVertexAttributeDescriptions =
        vertexAttributeDescriptions.data();
    createInfo.pVertexInputState = &vertexInputState;
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = params.inputAssembly.topology;
    inputAssemblyState.primitiveRestartEnable =
        params.inputAssembly.primitiveRestartEnable;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    auto tessellationState = vk::PipelineTessellationStateCreateInfo{};
    if (params.tessellation) {
      tessellationState.patchControlPoints =
          params.tessellation->patchControlPoints;
      createInfo.pTessellationState = &tessellationState;
    }
    auto viewportState = vk::PipelineViewportStateCreateInfo{};
    auto viewports = std::vector<vk::Viewport>{};
    auto scissors = std::vector<vk::Rect2D>{};
    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
    auto depthStencilState = vk::PipelineDepthStencilStateCreateInfo{};
    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    auto colorBlendAttachmentStates =
        std::vector<vk::PipelineColorBlendAttachmentState>{};
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
    auto dynamicStates = std::vector<vk::DynamicState>{};
    if (params.rasterization) {
      if (params.rasterization->viewports.index() == 0) {
        viewportState.viewportCount =
            std::get<0>(params.rasterization->viewports);
        dynamicStates.emplace_back(vk::DynamicState::eViewport);
      } else {
        viewports.reserve(std::get<1>(params.rasterization->viewports).size());
        for (auto &viewport : std::get<1>(params.rasterization->viewports)) {
          viewports.emplace_back(
              viewport.min().x(),
              viewport.min().y(),
              viewport.diagonal().x(),
              viewport.diagonal().y(),
              viewport.min().z(),
              viewport.max().z());
        }
        viewportState.viewportCount =
            static_cast<std::uint32_t>(viewports.size());
        viewportState.pViewports = viewports.data();
      }
      if (params.rasterization->scissors.index() == 0) {
        viewportState.scissorCount =
            std::get<0>(params.rasterization->scissors);
        dynamicStates.emplace_back(vk::DynamicState::eScissor);
      } else {
        scissors.reserve(std::get<1>(params.rasterization->scissors).size());
        for (auto &scissor : std::get<1>(params.rasterization->scissors)) {
          auto offset = vk::Offset2D{};
          offset.x = scissor.min().x();
          offset.y = scissor.min().y();
          auto extent = vk::Extent2D{};
          extent.width = static_cast<std::uint32_t>(scissor.diagonal().x());
          extent.height = static_cast<std::uint32_t>(scissor.diagonal().y());
          scissors.emplace_back(offset, extent);
        }
        viewportState.scissorCount =
            static_cast<std::uint32_t>(scissors.size());
        viewportState.pScissors = scissors.data();
      }
      createInfo.pViewportState = &viewportState;
      rasterizationState.depthClampEnable =
          params.rasterization->depthClampEnable;
      switch (params.rasterization->polygonMode.index()) {
      case 0: // PolygonModeFillState
        rasterizationState.polygonMode = vk::PolygonMode::eFill;
        rasterizationState.cullMode =
            std::get<0>(params.rasterization->polygonMode).cullMode;
        rasterizationState.frontFace =
            std::get<0>(params.rasterization->polygonMode).frontFace;
        rasterizationState.lineWidth = 1.0f;
        break;
      case 1: // PolygonModeLineState
        rasterizationState.polygonMode = vk::PolygonMode::eLine;
        rasterizationState.lineWidth =
            std::get<1>(params.rasterization->polygonMode).lineWidth;
        break;
      case 2: // PolygonModePointState
        rasterizationState.polygonMode = vk::PolygonMode::ePoint;
        rasterizationState.lineWidth = 1.0f;
        break;
      }
      if (params.rasterization->depthBias) {
        rasterizationState.depthBiasEnable = true;
        rasterizationState.depthBiasConstantFactor =
            params.rasterization->depthBias->constantFactor;
        rasterizationState.depthBiasClamp =
            params.rasterization->depthBias->clamp;
        rasterizationState.depthBiasSlopeFactor =
            params.rasterization->depthBias->slopeFactor;
      }
      createInfo.pRasterizationState = &rasterizationState;
      multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
      createInfo.pMultisampleState = &multisampleState;
      if (params.rasterization->depthTest) {
        depthStencilState.depthTestEnable = true;
        depthStencilState.depthWriteEnable =
            params.rasterization->depthTest->writeEnable;
        depthStencilState.depthCompareOp =
            params.rasterization->depthTest->compareOp;
      }
      if (params.rasterization->depthBoundsTest) {
        depthStencilState.depthBoundsTestEnable = true;
        depthStencilState.minDepthBounds =
            params.rasterization->depthBoundsTest->bounds.min()(0);
        depthStencilState.maxDepthBounds =
            params.rasterization->depthBoundsTest->bounds.max()(0);
      }
      if (params.rasterization->stencilTest) {
        depthStencilState.stencilTestEnable = true;
        depthStencilState.front.failOp =
            params.rasterization->stencilTest->front.failOp;
        depthStencilState.front.passOp =
            params.rasterization->stencilTest->front.passOp;
        depthStencilState.front.depthFailOp =
            params.rasterization->stencilTest->front.depthFailOp;
        depthStencilState.front.compareOp =
            params.rasterization->stencilTest->front.compareOp;
        depthStencilState.front.compareMask =
            params.rasterization->stencilTest->compareMasks[0];
        depthStencilState.front.writeMask =
            params.rasterization->stencilTest->writeMasks[0];
        depthStencilState.front.reference =
            params.rasterization->stencilTest->references[0];
        depthStencilState.back.failOp =
            params.rasterization->stencilTest->back.failOp;
        depthStencilState.back.passOp =
            params.rasterization->stencilTest->back.passOp;
        depthStencilState.back.depthFailOp =
            params.rasterization->stencilTest->back.depthFailOp;
        depthStencilState.back.compareOp =
            params.rasterization->stencilTest->back.compareOp;
        depthStencilState.back.compareMask =
            params.rasterization->stencilTest->compareMasks[1];
        depthStencilState.back.writeMask =
            params.rasterization->stencilTest->writeMasks[1];
        depthStencilState.back.reference =
            params.rasterization->stencilTest->references[1];
      }
      createInfo.pDepthStencilState = &depthStencilState;
      if (params.rasterization->blending) {
        colorBlendAttachmentStates.reserve(
            params.rasterization->blending->attachments.size());
        for (auto &attachment : params.rasterization->blending->attachments) {
          if (attachment.blending) {
            colorBlendAttachmentStates.emplace_back(
                true,
                attachment.blending->srcColorFactor,
                attachment.blending->dstColorFactor,
                attachment.blending->colorOp,
                attachment.blending->srcAlphaFactor,
                attachment.blending->dstAlphaFactor,
                attachment.blending->alphaOp);
          } else {
            colorBlendAttachmentStates.emplace_back();
          }
          colorBlendAttachmentStates.back().colorWriteMask =
              attachment.writeMask;
        }
        colorBlendState.attachmentCount =
            static_cast<std::uint32_t>(colorBlendAttachmentStates.size());
        colorBlendState.pAttachments = colorBlendAttachmentStates.data();
        colorBlendState.blendConstants =
            params.rasterization->blending->blendConstants;
      }
      createInfo.pColorBlendState = &colorBlendState;
      if (!dynamicStates.empty()) {
        dynamicState.dynamicStateCount =
            static_cast<std::uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
        createInfo.pDynamicState = &dynamicState;
      }
    } else {
      rasterizationState.rasterizerDiscardEnable = true;
      rasterizationState.lineWidth = 1.0f;
      createInfo.pRasterizationState = &rasterizationState;
    }
    createInfo.layout = params.layout->getHandle();
    createInfo.renderPass = params.renderPass->getHandle();
    createInfo.subpass = params.subpass;
    handle_ = device.createGraphicsPipelineUnique({}, createInfo).value;
  }
} // namespace mobula