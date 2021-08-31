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

      struct DepthBiasParams {
        float constantFactor;
        float slopeFactor;

        bool operator==(DepthBiasParams const &rhs) const = default;
      };

      struct DepthTestParams {
        bool writeEnable;
        vk::CompareOp compareOp;

        bool operator==(DepthTestParams const &rhs) const = default;
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

      struct ColorBlendParams {
        ColorFactor srcColorFactor;
        ColorFactor dstColorFactor;
        vk::BlendOp colorOp;
        AlphaFactor srcAlphaFactor;
        AlphaFactor dstAlphaFactor;
        vk::BlendOp alphaOp;
        std::array<float, 4> constants;
      };

      struct ColorParams {
        std::optional<ColorBlendParams> blend;
        std::vector<vk::ColorComponentFlags> writeMasks;

        bool operator==(ColorParams const &rhs) const = default;
      };

      struct RasterizationParams {
        std::optional<Bounds3f> viewport;
        std::optional<Bounds2i> scissor;
        vk::CullModeFlags cullMode;
        vk::FrontFace frontFace;
        std::optional<DepthBiasParams> depthBias;
        PipelineShaderStageParams fragmentStage;
        std::optional<DepthTestParams> depthTest;
        std::optional<StencilTestParams> stencilTest;
        std::optional<ColorParams> color;
      };

      vk::PipelineCreateFlagBits flags;
      PipelineLayout const *layout;
      RenderPass const *renderPass;
      std::uint32_t subpass;
      InputAssemblyParams inputAssembly;
      PipelineShaderStageParams vertexStage;
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

    export std::size_t
    hash_value(GraphicsPipelineParams::DepthBiasParams const &state) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, state.constantFactor);
      hash_combine(seed, state.slopeFactor);
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
        GraphicsPipelineParams::ColorBlendParams const &lhs,
        GraphicsPipelineParams::ColorBlendParams const &rhs) noexcept {
      return lhs.srcColorFactor == rhs.srcColorFactor &&
             lhs.dstColorFactor == rhs.dstColorFactor &&
             lhs.colorOp == rhs.colorOp &&
             lhs.srcAlphaFactor == rhs.srcAlphaFactor &&
             lhs.dstAlphaFactor == rhs.dstAlphaFactor &&
             lhs.alphaOp == rhs.alphaOp &&
             (!blendConstantsUsed(lhs.srcColorFactor) &&
                  !blendConstantsUsed(lhs.dstColorFactor) &&
                  !blendConstantsUsed(lhs.srcAlphaFactor) &&
                  !blendConstantsUsed(lhs.dstAlphaFactor) ||
              lhs.constants == rhs.constants);
    }

    export std::size_t hash_value(
        GraphicsPipelineParams::ColorBlendParams const &params) noexcept {
      auto seed = std::size_t{};
      boost::hash_combine(seed, params.srcColorFactor);
      boost::hash_combine(seed, params.dstColorFactor);
      boost::hash_combine(seed, params.colorOp);
      boost::hash_combine(seed, params.srcAlphaFactor);
      boost::hash_combine(seed, params.dstAlphaFactor);
      boost::hash_combine(seed, params.alphaOp);
      if (blendConstantsUsed(params.srcColorFactor) ||
          blendConstantsUsed(params.dstColorFactor) ||
          blendConstantsUsed(params.srcColorFactor) ||
          blendConstantsUsed(params.dstColorFactor)) {
        boost::hash_combine(seed, params.constants);
      }
      return seed;
    }

    export std::size_t
    hash_value(GraphicsPipelineParams::ColorParams const &params) noexcept {
      auto seed = std::size_t{};
      boost::hash_combine(seed, params.blend);
      for (auto writeMask : params.writeMasks) {
        boost::hash_combine(seed, static_cast<VkFlags>(writeMask));
      }
      return seed;
    }

    export bool operator==(
        GraphicsPipelineParams const &lhs,
        GraphicsPipelineParams const &rhs) noexcept {
      return lhs.flags == rhs.flags && lhs.layout == rhs.layout &&
             lhs.renderPass == rhs.renderPass && lhs.subpass == rhs.subpass &&
             lhs.inputAssembly == rhs.inputAssembly &&
             lhs.vertexStage == rhs.vertexStage &&
             lhs.rasterization.has_value() == rhs.rasterization.has_value() &&
             (!lhs.rasterization.has_value() ||
              lhs.rasterization->viewport == rhs.rasterization->viewport &&
                  lhs.rasterization->scissor == rhs.rasterization->scissor &&
                  lhs.rasterization->cullMode == rhs.rasterization->cullMode &&
                  lhs.rasterization->frontFace ==
                      rhs.rasterization->frontFace &&
                  lhs.rasterization->depthBias ==
                      rhs.rasterization->depthBias &&
                  lhs.rasterization->fragmentStage ==
                      rhs.rasterization->fragmentStage &&
                  (!lhs.renderPass->getParams()
                        .subpasses[lhs.subpass]
                        .depthStencilAttachment ||
                   lhs.rasterization->depthTest ==
                           rhs.rasterization->depthTest &&
                       lhs.rasterization->stencilTest ==
                           rhs.rasterization->stencilTest) &&
                  (lhs.renderPass->getParams()
                       .subpasses[lhs.subpass]
                       .colorAttachments.empty() ||
                   lhs.rasterization->color == rhs.rasterization->color));
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
      if (params.rasterization) {
        hash_combine(seed, params.rasterization->viewport);
        hash_combine(seed, params.rasterization->scissor);
        hash_combine(
            seed, static_cast<VkFlags>(params.rasterization->cullMode));
        hash_combine(seed, params.rasterization->frontFace);
        hash_combine(seed, params.rasterization->depthBias);
        hash_combine(seed, params.rasterization->fragmentStage);
        if (params.renderPass->getParams()
                .subpasses[params.subpass]
                .depthStencilAttachment) {
          hash_combine(seed, params.rasterization->depthTest);
          hash_combine(seed, params.rasterization->stencilTest);
        }
        if (!params.renderPass->getParams()
                 .subpasses[params.subpass]
                 .colorAttachments.empty()) {
          hash_combine(seed, params.rasterization->color);
        }
      }
      return seed;
    }
  } // namespace gpu
} // namespace mobula