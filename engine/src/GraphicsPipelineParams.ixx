// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.gpu:GraphicsPipelineParams;
import <array>;
import <optional>;
import <variant>;
import <vector>;
import mobula.util;
import :PipelineLayout;
import :PipelineShaderStageParams;
import :RenderPass;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Holds the parameters of a graphics pipeline.
     */
    export struct GraphicsPipelineParams {
      struct VertexBindingParams {
        std::uint32_t stride;
        vk::VertexInputRate inputRate;

        bool operator==(VertexBindingParams const &rhs) const = default;
      };

      struct VertexAttributeParams {
        std::uint32_t binding;
        vk::Format format;
        std::uint32_t offset;

        bool operator==(VertexAttributeParams const &rhs) const = default;
      };

      struct InputAssemblyParams {
        vk::PrimitiveTopology topology;
        bool primitiveRestartEnable;
        std::vector<VertexBindingParams> vertexBindings;
        std::vector<VertexAttributeParams> vertexAttributes;

        bool operator==(InputAssemblyParams const &rhs) const = default;
      };

      struct PolygonModeFillParams {
        vk::CullModeFlags cullMode;
        vk::FrontFace frontFace;

        bool operator==(PolygonModeFillParams const &rhs) const = default;
      };

      struct PolygonModeLineParams {
        float lineWidth;

        bool operator==(PolygonModeLineParams const &rhs) const = default;
      };

      struct PolygonModePointParams {
        bool operator==(PolygonModePointParams const &rhs) const = default;
      };

      struct DepthBiasParams {
        float constantFactor;
        float slopeFactor;
        float clamp;

        bool operator==(DepthBiasParams const &rhs) const = default;
      };

      struct DepthTestParams {
        bool writeEnable;
        vk::CompareOp compareOp;

        bool operator==(DepthTestParams const &rhs) const = default;
      };

      struct DepthBoundsTestParams {
        Bounds1f bounds;

        bool operator==(DepthBoundsTestParams const &rhs) const = default;
      };

      struct StencilOpParams {
        vk::StencilOp failOp;
        vk::StencilOp passOp;
        vk::StencilOp depthFailOp;
        vk::CompareOp compareOp;

        bool operator==(StencilOpParams const &rhs) const = default;
      };

      struct StencilTestParams {
        StencilOpParams front;
        StencilOpParams back;
        std::array<std::uint32_t, 2> compareMasks;
        std::array<std::uint32_t, 2> writeMasks;
        std::array<std::uint32_t, 2> references;

        bool operator==(StencilTestParams const &rhs) const = default;
      };

      enum class ColorFactor {
        zero = VK_BLEND_FACTOR_ZERO,
        one = VK_BLEND_FACTOR_ONE,
        srcColor = VK_BLEND_FACTOR_SRC_COLOR,
        oneMinusSrcColor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
        dstColor = VK_BLEND_FACTOR_DST_COLOR,
        oneMinusDstColor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
        srcAlpha = VK_BLEND_FACTOR_SRC_ALPHA,
        oneMinusSrcAlpha = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        dstAlpha = VK_BLEND_FACTOR_DST_ALPHA,
        oneMinusDstAlpha = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
        constantColor = VK_BLEND_FACTOR_CONSTANT_COLOR,
        oneMinusConstantColor = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
        constantAlpha = VK_BLEND_FACTOR_CONSTANT_ALPHA,
        oneMinusConstantAlpha = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA
      };

      enum class AlphaFactor {
        zero = VK_BLEND_FACTOR_ZERO,
        one = VK_BLEND_FACTOR_ONE,
        srcAlpha = VK_BLEND_FACTOR_SRC_ALPHA,
        oneMinusSrcAlpha = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        dstAlpha = VK_BLEND_FACTOR_DST_ALPHA,
        oneMinusDstAlpha = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
        constantAlpha = VK_BLEND_FACTOR_CONSTANT_ALPHA,
        oneMinusConstantAlpha = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA
      };

      struct ColorAttachmentBlendingParams {
        ColorFactor srcColorFactor;
        ColorFactor dstColorFactor;
        vk::BlendOp colorOp;
        AlphaFactor srcAlphaFactor;
        AlphaFactor dstAlphaFactor;
        vk::BlendOp alphaOp;

        bool
        operator==(ColorAttachmentBlendingParams const &rhs) const = default;
      };

      struct ColorAttachmentParams {
        std::optional<ColorAttachmentBlendingParams> blending;
        vk::ColorComponentFlags writeMask;

        bool operator==(ColorAttachmentParams const &rhs) const = default;
      };

      struct BlendingParams {
        std::vector<ColorAttachmentParams> attachments;
        std::array<float, 4> blendConstants;
      };

      struct RasterizationParams {
        std::variant<std::uint32_t, std::vector<Bounds3f>> viewports;
        std::variant<std::uint32_t, std::vector<Bounds2i>> scissors;
        bool depthClampEnable;
        std::variant<
            PolygonModeFillParams,
            PolygonModeLineParams,
            PolygonModePointParams>
            polygonMode;
        std::optional<DepthBiasParams> depthBias;
        PipelineShaderStageParams fragmentStage;
        std::optional<DepthTestParams> depthTest;
        std::optional<DepthBoundsTestParams> depthBoundsTest;
        std::optional<StencilTestParams> stencilTest;
        std::optional<BlendingParams> blending;
      };

      vk::PipelineCreateFlagBits flags;
      PipelineLayout const *layout;
      RenderPass const *renderPass;
      std::uint32_t subpass;
      InputAssemblyParams inputAssembly;
      PipelineShaderStageParams vertexStage;
      std::optional<PipelineShaderStageParams> geometryStage;
      std::optional<RasterizationParams> rasterization;
    };

    export std::size_t hash_value(
        GraphicsPipelineParams::VertexBindingParams const &binding) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, binding.stride);
      hash_combine(seed, binding.inputRate);
      return seed;
    }

    export std::size_t
    hash_value(GraphicsPipelineParams::VertexAttributeParams const
                   &attribute) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, attribute.binding);
      hash_combine(seed, attribute.format);
      hash_combine(seed, attribute.offset);
      return seed;
    }

    export std::size_t hash_value(
        GraphicsPipelineParams::InputAssemblyParams const &params) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, params.topology);
      hash_combine(seed, params.primitiveRestartEnable);
      hash_combine(seed, params.vertexBindings);
      hash_combine(seed, params.vertexAttributes);
      return seed;
    }

    export std::size_t hash_value(
        GraphicsPipelineParams::PolygonModeFillParams const &state) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, static_cast<VkFlags>(state.cullMode));
      hash_combine(seed, state.frontFace);
      return seed;
    }

    export std::size_t hash_value(
        GraphicsPipelineParams::PolygonModeLineParams const &state) noexcept {
      auto seed = std::size_t{};
      boost::hash_combine(seed, state.lineWidth);
      return seed;
    }

    export std::size_t hash_value(
        GraphicsPipelineParams::PolygonModePointParams const &state) noexcept {
      return 0;
    }

    export std::size_t
    hash_value(GraphicsPipelineParams::DepthBiasParams const &state) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, state.constantFactor);
      hash_combine(seed, state.slopeFactor);
      hash_combine(seed, state.clamp);
      return seed;
    }

    export std::size_t
    hash_value(GraphicsPipelineParams::DepthTestParams const &state) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, state.writeEnable);
      hash_combine(seed, state.compareOp);
      return seed;
    }

    export std::size_t hash_value(
        GraphicsPipelineParams::DepthBoundsTestParams const &state) noexcept {
      return hash_value(state.bounds);
    }

    export std::size_t
    hash_value(GraphicsPipelineParams::StencilOpParams const &state) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, state.passOp);
      hash_combine(seed, state.failOp);
      hash_combine(seed, state.depthFailOp);
      hash_combine(seed, state.compareOp);
      return seed;
    }

    export std::size_t hash_value(
        GraphicsPipelineParams::StencilTestParams const &state) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, state.front);
      hash_combine(seed, state.back);
      hash_combine(seed, state.compareMasks);
      hash_combine(seed, state.writeMasks);
      hash_combine(seed, state.references);
      return seed;
    }

    export std::size_t
    hash_value(GraphicsPipelineParams::ColorAttachmentBlendingParams const
                   &state) noexcept {
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
        GraphicsPipelineParams::ColorAttachmentParams const &state) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, state.blending);
      hash_combine(seed, static_cast<VkFlags>(state.writeMask));
      return seed;
    }

    export constexpr bool
    blendConstantsUsed(GraphicsPipelineParams::ColorFactor factor) noexcept {
      return factor == GraphicsPipelineParams::ColorFactor::constantColor ||
             factor ==
                 GraphicsPipelineParams::ColorFactor::oneMinusConstantColor ||
             factor == GraphicsPipelineParams::ColorFactor::constantAlpha ||
             factor ==
                 GraphicsPipelineParams::ColorFactor::oneMinusConstantAlpha;
    }

    export constexpr bool
    blendConstantsUsed(GraphicsPipelineParams::AlphaFactor factor) noexcept {
      return factor == GraphicsPipelineParams::AlphaFactor::constantAlpha ||
             factor ==
                 GraphicsPipelineParams::AlphaFactor::oneMinusConstantAlpha;
    }

    export bool operator==(
        GraphicsPipelineParams::BlendingParams const &lhs,
        GraphicsPipelineParams::BlendingParams const &rhs) noexcept {
      if (lhs.attachments.size() != rhs.attachments.size()) {
        return false;
      }
      auto blendConstants = false;
      for (auto i = std::size_t{}; i < lhs.attachments.size(); ++i) {
        if (lhs.attachments[i] != rhs.attachments[i]) {
          return false;
        }
        if (!blendConstants && lhs.attachments[i].blending &&
            (blendConstantsUsed(lhs.attachments[i].blending->srcColorFactor) ||
             blendConstantsUsed(lhs.attachments[i].blending->dstColorFactor) ||
             blendConstantsUsed(lhs.attachments[i].blending->srcAlphaFactor) ||
             blendConstantsUsed(lhs.attachments[i].blending->dstAlphaFactor))) {
          blendConstants = true;
        }
      }
      return !blendConstants || lhs.blendConstants == rhs.blendConstants;
    }

    export std::size_t
    hash_value(GraphicsPipelineParams::BlendingParams const &state) noexcept {
      using boost::hash_range;
      auto seed = std::size_t{};
      hash_range(seed, begin(state.attachments), end(state.attachments));
      hash_range(
          seed,
          state.blendConstants.data(),
          state.blendConstants.data() + state.blendConstants.size());
      return seed;
    }

    export bool operator==(
        GraphicsPipelineParams const &lhs,
        GraphicsPipelineParams const &rhs) noexcept {
      return lhs.flags == rhs.flags && lhs.layout == rhs.layout &&
             lhs.renderPass == rhs.renderPass && lhs.subpass == rhs.subpass &&
             lhs.inputAssembly == rhs.inputAssembly &&
             lhs.vertexStage == rhs.vertexStage &&
             lhs.geometryStage == rhs.geometryStage &&
             lhs.rasterization.has_value() == rhs.rasterization.has_value() &&
             (!lhs.rasterization.has_value() ||
              lhs.rasterization->viewports == rhs.rasterization->viewports &&
                  lhs.rasterization->scissors == rhs.rasterization->scissors &&
                  lhs.rasterization->depthClampEnable ==
                      rhs.rasterization->depthClampEnable &&
                  lhs.rasterization->polygonMode ==
                      rhs.rasterization->polygonMode &&
                  lhs.rasterization->depthBias ==
                      rhs.rasterization->depthBias &&
                  lhs.rasterization->fragmentStage ==
                      rhs.rasterization->fragmentStage &&
                  (!lhs.renderPass->getParams()
                        .subpasses[lhs.subpass]
                        .depthStencilAttachment ||
                   lhs.rasterization->depthTest ==
                           rhs.rasterization->depthTest &&
                       lhs.rasterization->depthBoundsTest ==
                           rhs.rasterization->depthBoundsTest &&
                       lhs.rasterization->stencilTest ==
                           rhs.rasterization->stencilTest) &&
                  (lhs.renderPass->getParams()
                       .subpasses[lhs.subpass]
                       .colorAttachments.empty() ||
                   lhs.rasterization->blending == rhs.rasterization->blending));
    }

    export std::size_t
    hash_value(GraphicsPipelineParams const &params) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, static_cast<VkFlags>(params.flags));
      hash_combine(seed, params.layout);
      hash_combine(seed, params.renderPass);
      hash_combine(seed, params.subpass);
      hash_combine(seed, params.inputAssembly);
      hash_combine(seed, params.vertexStage);
      hash_combine(seed, params.geometryStage);
      if (params.rasterization) {
        if (params.rasterization->viewports.index() == 0) {
          hash_combine(seed, std::get<0>(params.rasterization->viewports));
        } else {
          for (auto &viewport : std::get<1>(params.rasterization->viewports)) {
            hash_combine(seed, hash_value(viewport));
          }
        }
        if (params.rasterization->scissors.index() == 0) {
          hash_combine(seed, std::get<0>(params.rasterization->scissors));
        } else {
          for (auto &scissor : std::get<1>(params.rasterization->scissors)) {
            hash_combine(seed, hash_value(scissor));
          }
        }
        hash_combine(seed, params.rasterization->depthClampEnable);
        hash_combine(seed, params.rasterization->polygonMode);
        hash_combine(seed, params.rasterization->depthBias);
        hash_combine(seed, params.rasterization->fragmentStage);
        if (params.renderPass->getParams()
                .subpasses[params.subpass]
                .depthStencilAttachment) {
          hash_combine(seed, params.rasterization->depthTest);
          hash_combine(seed, params.rasterization->depthBoundsTest);
          hash_combine(seed, params.rasterization->stencilTest);
        }
        if (!params.renderPass->getParams()
                 .subpasses[params.subpass]
                 .colorAttachments.empty()) {
          hash_combine(seed, params.rasterization->blending);
        }
      }
      return seed;
    }
  } // namespace gpu
} // namespace mobula