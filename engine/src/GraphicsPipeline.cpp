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
  namespace {
    void writeStage(
        vk::PipelineShaderStageCreateInfo &dstStage,
        vk::SpecializationInfo &dstSpecializationInfo,
        std::vector<vk::SpecializationMapEntry> &dstMapEntries,
        std::vector<std::byte> &dstData,
        ShaderModuleCache &shaderModuleCache,
        vk::ShaderStageFlagBits shaderStage,
        PipelineShaderState const &shaderState) {
      dstStage.stage = shaderStage;
      dstStage.module =
          shaderModuleCache.create(shaderState.modulePath)->getHandle();
      dstStage.pName = shaderState.entryPoint.c_str();
      if (!shaderState.specializationConstants.empty()) {
        dstMapEntries.clear();
        dstMapEntries.reserve(shaderState.specializationConstants.size());
        dstData.clear();
        dstData.reserve(4 * shaderState.specializationConstants.size());
        auto offset = std::uint32_t{};
        for (auto &[id, value] : shaderState.specializationConstants) {
          dstMapEntries.emplace_back(id, offset, 4);
          dstData.resize(offset + 4);
          switch (value.index()) {
          case 0: {
            auto b = vk::Bool32{std::get<0>(value) ? 1u : 0u};
            std::memcpy(&dstData[offset], &b, 4);
            break;
          }
          case 1:
            std::memcpy(&dstData[offset], &std::get<1>(value), 4);
            break;
          case 2:
            std::memcpy(&dstData[offset], &std::get<2>(value), 4);
            break;
          case 3:
            std::memcpy(&dstData[offset], &std::get<3>(value), 4);
            break;
          }
          offset += 4;
        }
        dstSpecializationInfo.mapEntryCount =
            static_cast<std::uint32_t>(dstMapEntries.size());
        dstSpecializationInfo.pMapEntries = dstMapEntries.data();
        dstSpecializationInfo.dataSize = dstData.size();
        dstSpecializationInfo.pData = dstData.data();
        dstStage.pSpecializationInfo = &dstSpecializationInfo;
      }
    }
  } // namespace

  GraphicsPipeline::GraphicsPipeline(
      vk::Device device,
      ShaderModuleCache &shaderModuleCache,
      GraphicsPipelineParams const &params):
      params_{params} {
    auto createInfo = vk::GraphicsPipelineCreateInfo{};
    auto stages = std::vector<vk::PipelineShaderStageCreateInfo>{};
    stages.reserve(
        1 + 2 * params.tessellationState.has_value() +
        params.geometryShaderState.has_value() +
        params.rasterizationState.has_value());
    auto vertexStageSpecializationInfo = vk::SpecializationInfo{};
    auto vertexStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto vertexStageSpecializationData = std::vector<std::byte>{};
    writeStage(
        stages.emplace_back(),
        vertexStageSpecializationInfo,
        vertexStageSpecializationMapEntries,
        vertexStageSpecializationData,
        shaderModuleCache,
        vk::ShaderStageFlagBits::eVertex,
        params.vertexShaderState);
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
    if (params.tessellationState) {
      writeStage(
          stages.emplace_back(),
          tessellationControlStageSpecializationInfo,
          tessellationControlStageSpecializationMapEntries,
          tessellationControlStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eTessellationControl,
          params.tessellationState->controlShaderState);
      writeStage(
          stages.emplace_back(),
          tessellationEvaluationStageSpecializationInfo,
          tessellationEvaluationStageSpecializationMapEntries,
          tessellationEvaluationStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eTessellationEvaluation,
          params.tessellationState->evaluationShaderState);
    }
    auto geometryStageSpecializationInfo = vk::SpecializationInfo{};
    auto geometryStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto geometryStageSpecializationData = std::vector<std::byte>{};
    if (params.geometryShaderState) {
      writeStage(
          stages.emplace_back(),
          geometryStageSpecializationInfo,
          geometryStageSpecializationMapEntries,
          geometryStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eGeometry,
          *params.geometryShaderState);
    }
    auto fragmentStageSpecializationInfo = vk::SpecializationInfo{};
    auto fragmentStageSpecializationMapEntries =
        std::vector<vk::SpecializationMapEntry>{};
    auto fragmentStageSpecializationData = std::vector<std::byte>{};
    if (params.rasterizationState) {
      writeStage(
          stages.emplace_back(),
          fragmentStageSpecializationInfo,
          fragmentStageSpecializationMapEntries,
          fragmentStageSpecializationData,
          shaderModuleCache,
          vk::ShaderStageFlagBits::eFragment,
          params.rasterizationState->fragmentShaderState);
    }
    createInfo.stageCount = static_cast<std::uint32_t>(stages.size());
    createInfo.pStages = stages.data();
    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    auto vertexBindingDescriptions =
        std::vector<vk::VertexInputBindingDescription>{};
    vertexBindingDescriptions.reserve(
        params.inputAssemblyState.vertexBindings.size());
    for (auto i = std::uint32_t{};
         i < params.inputAssemblyState.vertexBindings.size();
         ++i) {
      auto &binding = params.inputAssemblyState.vertexBindings[i];
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
        params.inputAssemblyState.vertexAttributes.size());
    for (auto i = std::uint32_t{};
         i < params.inputAssemblyState.vertexAttributes.size();
         ++i) {
      auto &attribute = params.inputAssemblyState.vertexAttributes[i];
      vertexAttributeDescriptions.emplace_back(
          i, attribute.binding, attribute.format, attribute.offset);
    }
    vertexInputState.vertexAttributeDescriptionCount =
        static_cast<std::uint32_t>(vertexAttributeDescriptions.size());
    vertexInputState.pVertexAttributeDescriptions =
        vertexAttributeDescriptions.data();
    createInfo.pVertexInputState = &vertexInputState;
    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    inputAssemblyState.topology = params.inputAssemblyState.topology;
    inputAssemblyState.primitiveRestartEnable =
        params.inputAssemblyState.primitiveRestartEnable;
    createInfo.pInputAssemblyState = &inputAssemblyState;
    auto tessellationState = vk::PipelineTessellationStateCreateInfo{};
    if (params.tessellationState) {
      tessellationState.patchControlPoints =
          params.tessellationState->patchControlPoints;
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
    if (params.rasterizationState) {
      if (params.rasterizationState->viewports.index() == 0) {
        viewportState.viewportCount =
            std::get<0>(params.rasterizationState->viewports);
        dynamicStates.emplace_back(vk::DynamicState::eViewport);
      } else {
        viewports.reserve(
            std::get<1>(params.rasterizationState->viewports).size());
        for (auto &viewport :
             std::get<1>(params.rasterizationState->viewports)) {
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
      if (params.rasterizationState->scissors.index() == 0) {
        viewportState.scissorCount =
            std::get<0>(params.rasterizationState->scissors);
        dynamicStates.emplace_back(vk::DynamicState::eScissor);
      } else {
        scissors.reserve(
            std::get<1>(params.rasterizationState->scissors).size());
        for (auto &scissor : std::get<1>(params.rasterizationState->scissors)) {
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
          params.rasterizationState->depthClampEnable;
      switch (params.rasterizationState->polygonModeState.index()) {
      case 0: // PolygonModeFillState
        rasterizationState.polygonMode = vk::PolygonMode::eFill;
        rasterizationState.cullMode =
            std::get<0>(params.rasterizationState->polygonModeState).cullMode;
        rasterizationState.frontFace =
            std::get<0>(params.rasterizationState->polygonModeState).frontFace;
        rasterizationState.lineWidth = 1.0f;
        break;
      case 1: // PolygonModeLineState
        rasterizationState.polygonMode = vk::PolygonMode::eLine;
        rasterizationState.lineWidth =
            std::get<1>(params.rasterizationState->polygonModeState).lineWidth;
        break;
      case 2: // PolygonModePointState
        rasterizationState.polygonMode = vk::PolygonMode::ePoint;
        rasterizationState.lineWidth = 1.0f;
        break;
      }
      if (params.rasterizationState->depthBiasState) {
        rasterizationState.depthBiasEnable = true;
        rasterizationState.depthBiasConstantFactor =
            params.rasterizationState->depthBiasState->constantFactor;
        rasterizationState.depthBiasClamp =
            params.rasterizationState->depthBiasState->clamp;
        rasterizationState.depthBiasSlopeFactor =
            params.rasterizationState->depthBiasState->slopeFactor;
      }
      createInfo.pRasterizationState = &rasterizationState;
      multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
      createInfo.pMultisampleState = &multisampleState;
      if (params.rasterizationState->depthTestState) {
        depthStencilState.depthTestEnable = true;
        depthStencilState.depthWriteEnable =
            params.rasterizationState->depthTestState->writeEnable;
        depthStencilState.depthCompareOp =
            params.rasterizationState->depthTestState->compareOp;
      }
      if (params.rasterizationState->depthBoundsTestState) {
        depthStencilState.depthBoundsTestEnable = true;
        depthStencilState.minDepthBounds =
            params.rasterizationState->depthBoundsTestState->bounds.min()(0);
        depthStencilState.maxDepthBounds =
            params.rasterizationState->depthBoundsTestState->bounds.max()(0);
      }
      if (params.rasterizationState->stencilTestState) {
        depthStencilState.stencilTestEnable = true;
        depthStencilState.front.failOp =
            params.rasterizationState->stencilTestState->front.failOp;
        depthStencilState.front.passOp =
            params.rasterizationState->stencilTestState->front.passOp;
        depthStencilState.front.depthFailOp =
            params.rasterizationState->stencilTestState->front.depthFailOp;
        depthStencilState.front.compareOp =
            params.rasterizationState->stencilTestState->front.compareOp;
        depthStencilState.front.compareMask =
            params.rasterizationState->stencilTestState->compareMasks[0];
        depthStencilState.front.writeMask =
            params.rasterizationState->stencilTestState->writeMasks[0];
        depthStencilState.front.reference =
            params.rasterizationState->stencilTestState->references[0];
        depthStencilState.back.failOp =
            params.rasterizationState->stencilTestState->back.failOp;
        depthStencilState.back.passOp =
            params.rasterizationState->stencilTestState->back.passOp;
        depthStencilState.back.depthFailOp =
            params.rasterizationState->stencilTestState->back.depthFailOp;
        depthStencilState.back.compareOp =
            params.rasterizationState->stencilTestState->back.compareOp;
        depthStencilState.back.compareMask =
            params.rasterizationState->stencilTestState->compareMasks[1];
        depthStencilState.back.writeMask =
            params.rasterizationState->stencilTestState->writeMasks[1];
        depthStencilState.back.reference =
            params.rasterizationState->stencilTestState->references[1];
      }
      createInfo.pDepthStencilState = &depthStencilState;
      if (params.rasterizationState->colorBlendState) {
        colorBlendAttachmentStates.reserve(
            params.rasterizationState->colorBlendState->attachmentStates
                .size());
        for (auto &attachmentState :
             params.rasterizationState->colorBlendState->attachmentStates) {
          if (attachmentState.blendState) {
            colorBlendAttachmentStates.emplace_back(
                true,
                attachmentState.blendState->srcColorFactor,
                attachmentState.blendState->dstColorFactor,
                attachmentState.blendState->colorOp,
                attachmentState.blendState->srcAlphaFactor,
                attachmentState.blendState->dstAlphaFactor,
                attachmentState.blendState->alphaOp);
          } else {
            colorBlendAttachmentStates.emplace_back();
          }
          colorBlendAttachmentStates.back().colorWriteMask =
              attachmentState.writeMask;
        }
        colorBlendState.attachmentCount =
            static_cast<std::uint32_t>(colorBlendAttachmentStates.size());
        colorBlendState.pAttachments = colorBlendAttachmentStates.data();
        colorBlendState.blendConstants =
            params.rasterizationState->colorBlendState->blendConstants;
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