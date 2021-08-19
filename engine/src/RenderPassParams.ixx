// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:RenderPassParams;
import <vector>;
// clang-format on

namespace mobula {
  export struct RenderPassParams {
    struct AttachmentDescription {
      vk::AttachmentDescriptionFlags flags;
      vk::Format format;
      vk::AttachmentLoadOp loadOp;
      vk::AttachmentStoreOp storeOp;
      vk::AttachmentLoadOp stencilLoadOp;
      vk::AttachmentStoreOp stencilStoreOp;
      vk::ImageLayout initialLayout;
      vk::ImageLayout finalLayout;

      bool operator==(AttachmentDescription const &rhs) const = default;
    };

    struct AttachmentReference {
      std::uint32_t attachment;
      vk::ImageLayout layout;

      bool operator==(AttachmentReference const &rhs) const = default;
    };

    struct SubpassDescription {
      vk::PipelineBindPoint pipelineBindPoint;
      std::vector<AttachmentReference> inputAttachments;
      std::vector<AttachmentReference> colorAttachments;
      std::optional<AttachmentReference> depthStencilAttachment;
      std::vector<std::uint32_t> preserveAttachments;

      bool operator==(SubpassDescription const &rhs) const = default;
    };

    struct SubpassDependency {
      vk::DependencyFlags flags;
      std::uint32_t srcSubpass;
      std::uint32_t dstSubpass;
      vk::PipelineStageFlags srcStageMask;
      vk::PipelineStageFlags dstStageMask;
      vk::AccessFlags srcAccessMask;
      vk::AccessFlags dstAccessMask;

      bool operator==(SubpassDependency const &rhs) const = default;
    };

    std::vector<AttachmentDescription> attachments;
    std::vector<SubpassDescription> subpasses;
    std::vector<SubpassDependency> dependencies;

    bool operator==(RenderPassParams const &rhs) const = default;
  };

  export std::size_t
  hash_value(RenderPassParams::AttachmentDescription const &desc) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, static_cast<VkFlags>(desc.flags));
    hash_combine(seed, desc.format);
    hash_combine(seed, desc.loadOp);
    hash_combine(seed, desc.storeOp);
    hash_combine(seed, desc.stencilLoadOp);
    hash_combine(seed, desc.stencilStoreOp);
    hash_combine(seed, desc.initialLayout);
    hash_combine(seed, desc.finalLayout);
    return seed;
  }

  export std::size_t
  hash_value(RenderPassParams::AttachmentReference const &ref) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, ref.attachment);
    hash_combine(seed, ref.layout);
    return seed;
  }

  export std::size_t
  hash_value(RenderPassParams::SubpassDescription const &desc) noexcept {
    using boost::hash_combine;
    using boost::hash_range;
    auto seed = std::size_t{};
    hash_combine(seed, desc.pipelineBindPoint);
    hash_range(seed, begin(desc.inputAttachments), end(desc.inputAttachments));
    hash_range(seed, begin(desc.colorAttachments), end(desc.colorAttachments));
    if (desc.depthStencilAttachment) {
      hash_combine(seed, *desc.depthStencilAttachment);
    }
    hash_range(
        seed, begin(desc.preserveAttachments), end(desc.preserveAttachments));
    return seed;
  }

  export std::size_t
  hash_value(RenderPassParams::SubpassDependency const &dep) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, static_cast<VkFlags>(dep.flags));
    hash_combine(seed, dep.srcSubpass);
    hash_combine(seed, dep.dstSubpass);
    hash_combine(seed, static_cast<VkFlags>(dep.srcStageMask));
    hash_combine(seed, static_cast<VkFlags>(dep.dstStageMask));
    hash_combine(seed, static_cast<VkFlags>(dep.srcAccessMask));
    hash_combine(seed, static_cast<VkFlags>(dep.dstAccessMask));
    return seed;
  }

  export std::size_t hash_value(RenderPassParams const &params) noexcept {
    using boost::hash_range;
    auto seed = std::size_t{};
    hash_range(seed, begin(params.attachments), end(params.attachments));
    hash_range(seed, begin(params.subpasses), end(params.subpasses));
    hash_range(seed, begin(params.dependencies), end(params.dependencies));
    return seed;
  }
} // namespace mobula