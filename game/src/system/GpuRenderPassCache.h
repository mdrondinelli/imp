#pragma once

#include <algorithm>
#include <list>
#include <span>
#include <unordered_map>
#include <vector>
#include <mutex>

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

namespace imp {
  struct GpuAttachmentDescription {
    vk::AttachmentDescriptionFlags flags;
    vk::Format format;
    vk::SampleCountFlagBits samples;
    vk::AttachmentLoadOp loadOp;
    vk::AttachmentStoreOp storeOp;
    vk::AttachmentLoadOp stencilLoadOp;
    vk::AttachmentStoreOp stencilStoreOp;
    vk::ImageLayout initialLayout;
    vk::ImageLayout finalLayout;

    explicit operator vk::AttachmentDescription() const noexcept {
      return vk::AttachmentDescription{
          flags,
          format,
          samples,
          loadOp,
          storeOp,
          stencilLoadOp,
          stencilStoreOp,
          initialLayout,
          finalLayout};
    }

    friend bool operator==(
        GpuAttachmentDescription const &lhs,
        GpuAttachmentDescription const &rhs) = default;
  };

  inline std::size_t hash_value(GpuAttachmentDescription const &desc) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, static_cast<VkFlags>(desc.flags));
    boost::hash_combine(seed, desc.format);
    boost::hash_combine(seed, desc.samples);
    boost::hash_combine(seed, desc.loadOp);
    boost::hash_combine(seed, desc.storeOp);
    boost::hash_combine(seed, desc.stencilLoadOp);
    boost::hash_combine(seed, desc.stencilStoreOp);
    boost::hash_combine(seed, desc.initialLayout);
    boost::hash_combine(seed, desc.finalLayout);
    return seed;
  }

  struct GpuAttachmentReference {
    std::uint32_t attachment;
    vk::ImageLayout layout;

    explicit operator vk::AttachmentReference() const noexcept {
      return vk::AttachmentReference{attachment, layout};
    }

    friend bool operator==(
        GpuAttachmentReference const &lhs,
        GpuAttachmentReference const &rhs) = default;
  };

  inline std::size_t hash_value(GpuAttachmentReference const &ref) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, ref.attachment);
    boost::hash_combine(seed, ref.layout);
    return seed;
  }

  struct GpuSubpassDescription {
    vk::PipelineBindPoint pipelineBindPoint;
    std::span<GpuAttachmentReference const> inputAttachments;
    std::span<GpuAttachmentReference const> colorAttachments;
    std::span<GpuAttachmentReference const> resolveAttachments;
    GpuAttachmentReference const *depthStencilAttachment;
    std::span<std::uint32_t const> preserveAttachments;
  };

  inline bool operator==(
      GpuSubpassDescription const &lhs,
      GpuSubpassDescription const &rhs) noexcept {
    return lhs.pipelineBindPoint == rhs.pipelineBindPoint &&
           (lhs.inputAttachments.data() == rhs.inputAttachments.data() &&
                lhs.inputAttachments.size() == rhs.inputAttachments.size() ||
            std::ranges::equal(lhs.inputAttachments, rhs.inputAttachments)) &&
           (lhs.colorAttachments.data() == rhs.colorAttachments.data() &&
                lhs.colorAttachments.size() == rhs.colorAttachments.size() ||
            std::ranges::equal(lhs.colorAttachments, rhs.colorAttachments)) &&
           (lhs.resolveAttachments.data() == rhs.resolveAttachments.data() &&
                lhs.resolveAttachments.size() ==
                    rhs.resolveAttachments.size() ||
            std::ranges::equal(
                lhs.resolveAttachments, rhs.resolveAttachments)) &&
           (lhs.depthStencilAttachment == rhs.depthStencilAttachment ||
            *lhs.depthStencilAttachment == *rhs.depthStencilAttachment) &&
           (lhs.preserveAttachments.data() == rhs.preserveAttachments.data() &&
                lhs.preserveAttachments.size() ==
                    rhs.preserveAttachments.size() ||
            std::ranges::equal(
                lhs.preserveAttachments, rhs.preserveAttachments));
  }

  inline bool operator!=(
      GpuSubpassDescription const &lhs,
      GpuSubpassDescription const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline std::size_t hash_value(GpuSubpassDescription const &desc) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, desc.pipelineBindPoint);
    boost::hash_range(
        seed, desc.inputAttachments.begin(), desc.inputAttachments.end());
    boost::hash_range(
        seed, desc.colorAttachments.begin(), desc.colorAttachments.end());
    boost::hash_range(
        seed, desc.resolveAttachments.begin(), desc.resolveAttachments.end());
    if (desc.depthStencilAttachment) {
      boost::hash_combine(seed, *desc.depthStencilAttachment);
    }
    boost::hash_range(
        seed, desc.preserveAttachments.begin(), desc.preserveAttachments.end());
    return seed;
  }

  struct GpuSubpassDependency {
    std::uint32_t srcSubpass;
    std::uint32_t dstSubpass;
    vk::PipelineStageFlags srcStageMask;
    vk::PipelineStageFlags dstStageMask;
    vk::AccessFlags srcAccessMask;
    vk::AccessFlags dstAccessMask;
    vk::DependencyFlags dependencyFlags;

    explicit operator vk::SubpassDependency() const noexcept {
      return vk::SubpassDependency{
          srcSubpass,
          dstSubpass,
          srcStageMask,
          dstStageMask,
          srcAccessMask,
          dstAccessMask,
          dependencyFlags};
    }

    friend bool operator==(
        GpuSubpassDependency const &lhs,
        GpuSubpassDependency const &rhs) = default;
  };

  inline std::size_t hash_value(GpuSubpassDependency const &dep) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, dep.srcSubpass);
    boost::hash_combine(seed, dep.dstSubpass);
    boost::hash_combine(seed, static_cast<VkFlags>(dep.srcStageMask));
    boost::hash_combine(seed, static_cast<VkFlags>(dep.dstStageMask));
    return seed;
  }

  struct GpuRenderPassCreateInfo {
    std::span<GpuAttachmentDescription const> attachments;
    std::span<GpuSubpassDescription const> subpasses;
    std::span<GpuSubpassDependency const> dependencies;
  };

  inline bool operator==(
      GpuRenderPassCreateInfo const &lhs,
      GpuRenderPassCreateInfo const &rhs) noexcept {
    return (lhs.attachments.data() == rhs.attachments.data() &&
                lhs.attachments.size() == rhs.attachments.size() ||
            std::ranges::equal(lhs.attachments, rhs.attachments)) &&
           (lhs.subpasses.data() == rhs.subpasses.data() &&
                lhs.subpasses.size() == rhs.subpasses.size() ||
            std::ranges::equal(lhs.attachments, rhs.attachments)) &&
           (lhs.dependencies.data() == rhs.dependencies.data() &&
                lhs.dependencies.size() == rhs.dependencies.size() ||
            std::ranges::equal(lhs.dependencies, rhs.dependencies));
  }

  inline bool operator!=(
      GpuRenderPassCreateInfo const &lhs,
      GpuRenderPassCreateInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline std::size_t
  hash_value(GpuRenderPassCreateInfo const &createInfo) noexcept {
    auto seed = std::size_t{};
    boost::hash_range(
        seed, createInfo.attachments.begin(), createInfo.attachments.end());
    boost::hash_range(
        seed, createInfo.subpasses.begin(), createInfo.subpasses.end());
    boost::hash_range(
        seed, createInfo.dependencies.begin(), createInfo.dependencies.end());
    return seed;
  }

  class GpuRenderPassCache {
  public:
    explicit GpuRenderPassCache(vk::Device device);

    vk::RenderPass create(GpuRenderPassCreateInfo const &createInfo);

  private:
    vk::Device device_;
    std::list<std::vector<GpuAttachmentDescription>> attachmentDescriptions_;
    std::list<std::vector<GpuAttachmentReference>> attachmentReferences_;
    std::list<std::vector<std::uint32_t>> preserveAttachments_;
    std::list<std::vector<GpuSubpassDescription>> subpassDescriptions_;
    std::list<std::vector<GpuSubpassDependency>> subpassDependencies_;
    std::unordered_map<
        GpuRenderPassCreateInfo,
        vk::UniqueRenderPass,
        boost::hash<GpuRenderPassCreateInfo>>
        renderPasses_;
    std::mutex mutex_;
  };
} // namespace imp