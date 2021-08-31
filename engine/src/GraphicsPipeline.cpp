// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.gpu;
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
  namespace gpu {
    GraphicsPipeline::GraphicsPipeline(
        vk::Device device,
        ShaderModuleCache &shaderModuleCache,
        GraphicsPipelineParams const &params):
        params_{params} {
      auto createInfo = vk::GraphicsPipelineCreateInfo{};
      auto stages = std::vector<vk::PipelineShaderStageCreateInfo>{};
      stages.reserve(1 + params.rasterization.has_value());
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
      auto viewportState = vk::PipelineViewportStateCreateInfo{};
      auto viewport = vk::Viewport{};
      auto scissor = vk::Rect2D{};
      auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{};
      auto multisampleState = vk::PipelineMultisampleStateCreateInfo{};
      auto depthStencilState = vk::PipelineDepthStencilStateCreateInfo{};
      auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{};
      auto colorBlendAttachmentStates =
          std::vector<vk::PipelineColorBlendAttachmentState>{};
      auto dynamicState = vk::PipelineDynamicStateCreateInfo{};
      auto dynamicStates = std::vector<vk::DynamicState>{};
      if (params.rasterization) {
        viewportState.viewportCount = 1;
        if (params.rasterization->viewport) {
          viewport.x = params.rasterization->viewport->min.x();
          viewport.y = params.rasterization->viewport->min.y();
          viewport.width = params.rasterization->viewport->max.x() -
                           params.rasterization->viewport->min.x();
          viewport.height = params.rasterization->viewport->max.y() -
                            params.rasterization->viewport->min.y();
          viewport.minDepth = params.rasterization->viewport->min.z();
          viewport.maxDepth = params.rasterization->viewport->max.z();
          viewportState.pViewports = &viewport;
        } else {
          dynamicStates.emplace_back(vk::DynamicState::eViewport);
        }
        viewportState.scissorCount = 1;
        if (params.rasterization->scissor) {
          scissor.offset.x = params.rasterization->scissor->min.x();
          scissor.offset.y = params.rasterization->scissor->min.y();
          scissor.extent.width = static_cast<std::uint32_t>(
              params.rasterization->scissor->max.x() -
              params.rasterization->scissor->min.x());
          scissor.extent.height = static_cast<std::uint32_t>(
              params.rasterization->scissor->max.y() -
              params.rasterization->scissor->min.y());
          viewportState.pScissors = &scissor;
        } else {
          dynamicStates.emplace_back(vk::DynamicState::eScissor);
        }
        createInfo.pViewportState = &viewportState;
        rasterizationState.polygonMode = vk::PolygonMode::eFill;
        rasterizationState.cullMode = params.rasterization->cullMode;
        rasterizationState.frontFace = params.rasterization->frontFace;
        if (params.rasterization->depthBias) {
          rasterizationState.depthBiasEnable = true;
          rasterizationState.depthBiasConstantFactor =
              params.rasterization->depthBias->constantFactor;
          rasterizationState.depthBiasSlopeFactor =
              params.rasterization->depthBias->slopeFactor;
        }
        rasterizationState.lineWidth = 1.0f;
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
        if (params.rasterization->color) {
          colorBlendAttachmentStates.reserve(
              params.rasterization->color->writeMasks.size());
          if (params.rasterization->color->blend) {
            for (auto writeMask : params.rasterization->color->writeMasks) {
              colorBlendAttachmentStates.emplace_back(
                  true,
                  static_cast<vk::BlendFactor>(
                      params.rasterization->color->blend->srcColorFactor),
                  static_cast<vk::BlendFactor>(
                      params.rasterization->color->blend->dstColorFactor),
                  params.rasterization->color->blend->colorOp,
                  static_cast<vk::BlendFactor>(
                      params.rasterization->color->blend->srcAlphaFactor),
                  static_cast<vk::BlendFactor>(
                      params.rasterization->color->blend->dstAlphaFactor),
                  params.rasterization->color->blend->alphaOp,
                  writeMask);
            }
            colorBlendState.blendConstants =
                params.rasterization->color->blend->constants;
          } else {
            for (auto writeMask : params.rasterization->color->writeMasks) {
              colorBlendAttachmentStates.emplace_back(
                  false,
                  vk::BlendFactor::eZero,
                  vk::BlendFactor::eZero,
                  vk::BlendOp::eAdd,
                  vk::BlendFactor::eZero,
                  vk::BlendFactor::eZero,
                  vk::BlendOp::eAdd,
                  writeMask);
            }
          }
          colorBlendState.attachmentCount =
              static_cast<std::uint32_t>(colorBlendAttachmentStates.size());
          colorBlendState.pAttachments = colorBlendAttachmentStates.data();
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
  } // namespace gpu
} // namespace mobula