// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:GraphicsPipelineParams;
import <array>;
import <optional>;
import <variant>;
import <vector>;
import mobula.engine.util;
import :PipelineLayout;
import :PipelineShaderState;
import :RenderPass;
// clang-format on

namespace mobula {
  export struct GraphicsPipelineParams {
    struct VertexBinding {
      std::uint32_t stride;
      vk::VertexInputRate inputRate;

      friend bool
      operator==(VertexBinding const &lhs, VertexBinding const &rhs) = default;
    };

    struct VertexAttribute {
      std::uint32_t binding;
      vk::Format format;
      std::uint32_t offset;

      friend bool operator==(
          VertexAttribute const &lhs, VertexAttribute const &rhs) = default;
    };

    struct InputAssemblyState {
      vk::PrimitiveTopology topology;
      bool primitiveRestartEnable;
      std::vector<VertexBinding> vertexBindings;
      std::vector<VertexAttribute> vertexAttributes;

      friend bool operator==(
          InputAssemblyState const &lhs,
          InputAssemblyState const &rhs) = default;
    };

    struct TessellationState {
      std::uint32_t patchControlPoints;
      PipelineShaderState controlShaderState;
      PipelineShaderState evaluationShaderState;

      friend bool operator==(
          TessellationState const &lhs, TessellationState const &rhs) = default;
    };

    struct PolygonModeFillState {
      vk::CullModeFlags cullMode;
      vk::FrontFace frontFace;

      friend bool operator==(
          PolygonModeFillState const &lhs,
          PolygonModeFillState const &rhs) = default;
    };

    struct PolygonModeLineState {
      float lineWidth;

      friend bool operator==(
          PolygonModeLineState const &lhs,
          PolygonModeLineState const &rhs) = default;
    };

    struct PolygonModePointState {
      friend bool operator==(
          PolygonModePointState const &lhs,
          PolygonModePointState const &rhs) = default;
    };

    struct DepthBiasState {
      float constantFactor;
      float slopeFactor;
      float clamp;

      friend bool operator==(
          DepthBiasState const &lhs, DepthBiasState const &rhs) = default;
    };

    struct DepthTestState {
      bool writeEnable;
      vk::CompareOp compareOp;

      friend bool operator==(
          DepthTestState const &lhs, DepthTestState const &rhs) = default;
    };

    struct DepthBoundsTestState {
      AlignedBox1f bounds;

      friend bool operator==(
          DepthBoundsTestState const &lhs,
          DepthBoundsTestState const &rhs) = default;
    };

    struct StencilOpState {
      vk::StencilOp failOp;
      vk::StencilOp passOp;
      vk::StencilOp depthFailOp;
      vk::CompareOp compareOp;

      friend bool operator==(
          StencilOpState const &lhs, StencilOpState const &rhs) = default;
    };

    struct StencilTestState {
      StencilOpState front;
      StencilOpState back;
      std::array<std::uint32_t, 2> compareMasks;
      std::array<std::uint32_t, 2> writeMasks;
      std::array<std::uint32_t, 2> references;

      friend bool operator==(
          StencilTestState const &lhs, StencilTestState const &rhs) = default;
    };

    struct ColorAttachmentBlendState {
      vk::BlendFactor srcColorFactor;
      vk::BlendFactor dstColorFactor;
      vk::BlendOp colorOp;
      vk::BlendFactor srcAlphaFactor;
      vk::BlendFactor dstAlphaFactor;
      vk::BlendOp alphaOp;

      friend bool operator==(
          ColorAttachmentBlendState const &lhs,
          ColorAttachmentBlendState const &rhs) = default;
    };

    struct ColorAttachmentState {
      std::optional<ColorAttachmentBlendState> blendState;
      vk::ColorComponentFlags writeMask;

      friend bool operator==(
          ColorAttachmentState const &lhs,
          ColorAttachmentState const &rhs) = default;
    };

    struct ColorBlendState {
      std::vector<ColorAttachmentState> attachmentStates;
      std::array<float, 4> blendConstants;

      friend bool operator==(
          ColorBlendState const &lhs, ColorBlendState const &rhs) = default;
    };

    struct RasterizationState {
      std::variant<std::uint32_t, std::vector<AlignedBox3f>> viewports;
      std::variant<std::uint32_t, std::vector<AlignedBox2i>> scissors;
      bool depthClampEnable;
      std::variant<
          PolygonModeFillState,
          PolygonModeLineState,
          PolygonModePointState>
          polygonModeState;
      std::optional<DepthBiasState> depthBiasState;
      PipelineShaderState fragmentShaderState;
      std::optional<DepthTestState> depthTestState;
      std::optional<DepthBoundsTestState> depthBoundsTestState;
      std::optional<StencilTestState> stencilTestState;
      std::optional<ColorBlendState> colorBlendState;
    };

    vk::PipelineCreateFlagBits flags;
    PipelineLayout const *layout;
    RenderPass const *renderPass;
    std::uint32_t subpass;
    InputAssemblyState inputAssemblyState;
    PipelineShaderState vertexShaderState;
    std::optional<TessellationState> tessellationState;
    std::optional<PipelineShaderState> geometryShaderState;
    std::optional<RasterizationState> rasterizationState;
  };

  export std::size_t
  hash_value(GraphicsPipelineParams::VertexBinding const &binding) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, binding.stride);
    hash_combine(seed, binding.inputRate);
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::VertexAttribute const &attribute) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, attribute.binding);
    hash_combine(seed, attribute.format);
    hash_combine(seed, attribute.offset);
    return seed;
  }

  export std::size_t
  hash_value(GraphicsPipelineParams::InputAssemblyState const &state) noexcept {
    using boost::hash_combine;
    using boost::hash_range;
    auto seed = std::size_t{};
    hash_combine(seed, state.topology);
    hash_combine(seed, state.primitiveRestartEnable);
    hash_range(seed, begin(state.vertexBindings), end(state.vertexBindings));
    hash_range(
        seed, begin(state.vertexAttributes), end(state.vertexAttributes));
    return seed;
  }

  export std::size_t
  hash_value(GraphicsPipelineParams::TessellationState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.patchControlPoints);
    hash_combine(seed, state.controlShaderState);
    hash_combine(seed, state.evaluationShaderState);
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::PolygonModeFillState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, static_cast<VkFlags>(state.cullMode));
    hash_combine(seed, state.frontFace);
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::PolygonModeLineState const &state) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, state.lineWidth);
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::PolygonModePointState const &state) noexcept {
    return 0;
  }

  export std::size_t
  hash_value(GraphicsPipelineParams::DepthBiasState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.constantFactor);
    hash_combine(seed, state.slopeFactor);
    hash_combine(seed, state.clamp);
    return seed;
  }

  export std::size_t
  hash_value(GraphicsPipelineParams::DepthTestState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.writeEnable);
    hash_combine(seed, state.compareOp);
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::DepthBoundsTestState const &state) noexcept {
    return hash_value(state.bounds);
  }

  export std::size_t
  hash_value(GraphicsPipelineParams::StencilOpState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.passOp);
    hash_combine(seed, state.failOp);
    hash_combine(seed, state.depthFailOp);
    hash_combine(seed, state.compareOp);
    return seed;
  }

  export std::size_t
  hash_value(GraphicsPipelineParams::StencilTestState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.front);
    hash_combine(seed, state.back);
    hash_combine(seed, state.compareMasks);
    hash_combine(seed, state.writeMasks);
    hash_combine(seed, state.references);
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::ColorAttachmentBlendState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.srcColorFactor);
    hash_combine(seed, state.dstColorFactor);
    hash_combine(seed, state.colorOp);
    hash_combine(seed, state.srcAlphaFactor);
    hash_combine(seed, state.dstAlphaFactor);
    hash_combine(seed, state.alphaOp);
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::ColorAttachmentState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.blendState);
    hash_combine(seed, static_cast<VkFlags>(state.writeMask));
    return seed;
  }

  export std::size_t
  hash_value(GraphicsPipelineParams::ColorBlendState const &state) noexcept {
    using boost::hash_range;
    auto seed = std::size_t{};
    hash_range(
        seed, begin(state.attachmentStates), end(state.attachmentStates));
    hash_range(
        seed,
        state.blendConstants.data(),
        state.blendConstants.data() + state.blendConstants.size());
    return seed;
  }

  export std::size_t hash_value(
      GraphicsPipelineParams::RasterizationState const &state,
      RenderPass const &renderPass,
      std::uint32_t subpass) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    if (state.viewports.index() == 0) {
      hash_combine(seed, std::get<0>(state.viewports));
    } else {
      for (auto &viewport : std::get<1>(state.viewports)) {
        hash_combine(seed, hash_value(viewport));
      }
    }
    if (state.scissors.index() == 0) {
      hash_combine(seed, std::get<0>(state.scissors));
    } else {
      for (auto &scissor : std::get<1>(state.scissors)) {
        hash_combine(seed, hash_value(scissor));
      }
    }
    hash_combine(seed, state.depthClampEnable);
    hash_combine(seed, state.polygonModeState);
    hash_combine(seed, state.depthBiasState);
    hash_combine(seed, state.fragmentShaderState);
    if (renderPass.getParams().subpasses[subpass].depthStencilAttachment) {
      hash_combine(seed, state.depthTestState);
      hash_combine(seed, state.depthBoundsTestState);
      hash_combine(seed, state.stencilTestState);
    }
    if (!renderPass.getParams().subpasses[subpass].colorAttachments.empty()) {
      hash_combine(seed, state.colorBlendState);
    }
    return seed;
  }

  export bool operator==(
      GraphicsPipelineParams const &lhs,
      GraphicsPipelineParams const &rhs) noexcept {
    if (lhs.flags != rhs.flags || lhs.layout != rhs.layout ||
        lhs.renderPass != rhs.renderPass || lhs.subpass != rhs.subpass ||
        lhs.inputAssemblyState != rhs.inputAssemblyState ||
        lhs.vertexShaderState != rhs.vertexShaderState ||
        lhs.tessellationState != rhs.tessellationState ||
        lhs.geometryShaderState != rhs.geometryShaderState) {
      return false;
    }
    return lhs.rasterizationState.has_value() ==
               rhs.rasterizationState.has_value() &&
           (!lhs.rasterizationState.has_value() ||
            lhs.rasterizationState->viewports ==
                    rhs.rasterizationState->viewports &&
                lhs.rasterizationState->scissors ==
                    rhs.rasterizationState->scissors &&
                lhs.rasterizationState->depthClampEnable ==
                    rhs.rasterizationState->depthClampEnable &&
                lhs.rasterizationState->polygonModeState ==
                    rhs.rasterizationState->polygonModeState &&
                lhs.rasterizationState->depthBiasState ==
                    rhs.rasterizationState->depthBiasState &&
                lhs.rasterizationState->fragmentShaderState ==
                    rhs.rasterizationState->fragmentShaderState &&
                (!lhs.renderPass->getParams()
                      .subpasses[lhs.subpass]
                      .depthStencilAttachment ||
                 lhs.rasterizationState->depthTestState ==
                         rhs.rasterizationState->depthTestState &&
                     lhs.rasterizationState->depthBoundsTestState ==
                         rhs.rasterizationState->depthBoundsTestState &&
                     lhs.rasterizationState->stencilTestState ==
                         rhs.rasterizationState->stencilTestState) &&
                (lhs.renderPass->getParams()
                     .subpasses[lhs.subpass]
                     .colorAttachments.empty() ||
                 lhs.rasterizationState->colorBlendState ==
                     rhs.rasterizationState->colorBlendState));
  }

  export std::size_t hash_value(GraphicsPipelineParams const &params) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, static_cast<VkFlags>(params.flags));
    hash_combine(seed, params.layout);
    hash_combine(seed, params.renderPass);
    hash_combine(seed, params.subpass);
    hash_combine(seed, params.inputAssemblyState);
    hash_combine(seed, params.vertexShaderState);
    hash_combine(seed, params.tessellationState);
    hash_combine(seed, params.geometryShaderState);
    if (params.rasterizationState) {
      hash_combine(
          seed,
          hash_value(
              *params.rasterizationState, *params.renderPass, params.subpass));
    }
    return seed;
  }
} // namespace mobula