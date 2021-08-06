module;

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

export module mobula.engine.gpu;

// clang-format off
import <forward_list>;
import <mutex>;
import <span>;
import <unordered_map>;
import <vector>;
// clang-format on

namespace mobula {
  export struct GpuAttachmentDescription {
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

    auto operator==(GpuAttachmentDescription const &rhs) const
        -> bool = default;
  };

  export auto hash_value(GpuAttachmentDescription const &desc) noexcept
      -> std::size_t;

  export struct GpuAttachmentReference {
    std::uint32_t attachment;
    vk::ImageLayout layout;

    explicit operator vk::AttachmentReference() const noexcept {
      return vk::AttachmentReference{attachment, layout};
    }

    auto operator==(GpuAttachmentReference const &rhs) const -> bool = default;
  };

  export auto hash_value(GpuAttachmentReference const &ref) noexcept
      -> std::size_t;

  export struct GpuSubpassDescription {
    vk::PipelineBindPoint pipelineBindPoint;
    std::span<GpuAttachmentReference const> inputAttachments;
    std::span<GpuAttachmentReference const> colorAttachments;
    std::span<GpuAttachmentReference const> resolveAttachments;
    GpuAttachmentReference const *depthStencilAttachment;
    std::span<std::uint32_t const> preserveAttachments;
  };

  export auto operator==(
      GpuSubpassDescription const &lhs,
      GpuSubpassDescription const &rhs) noexcept -> bool;

  export auto operator!=(
      GpuSubpassDescription const &lhs,
      GpuSubpassDescription const &rhs) noexcept -> bool;

  export auto hash_value(GpuSubpassDescription const &desc) noexcept
      -> std::size_t;

  export struct GpuSubpassDependency {
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

    auto operator==(GpuSubpassDependency const &rhs) const -> bool = default;
  };

  export auto hash_value(GpuSubpassDependency const &dep) noexcept
      -> std::size_t;

  export struct GpuRenderPassCreateInfo {
    std::span<GpuAttachmentDescription const> attachments;
    std::span<GpuSubpassDescription const> subpasses;
    std::span<GpuSubpassDependency const> dependencies;
  };

  export auto operator==(
      GpuRenderPassCreateInfo const &lhs,
      GpuRenderPassCreateInfo const &rhs) noexcept -> bool;

  export auto operator!=(
      GpuRenderPassCreateInfo const &lhs,
      GpuRenderPassCreateInfo const &rhs) noexcept -> bool;

  export auto hash_value(GpuRenderPassCreateInfo const &createInfo) noexcept
      -> std::size_t;

  /**
   * A class that wraps a vk::RenderPass.
   */
  export class GpuRenderPass {
  public:
    friend class GpuRenderPassCache;

    /**
     * @param createInfo the createInfo used to create the render pass.
     * @param handle the handle to the render pass.
     */
    explicit GpuRenderPass(
        GpuRenderPassCreateInfo const &createInfo,
        vk::UniqueRenderPass handle) noexcept;

    GpuRenderPass(GpuRenderPass &&rhs) = delete;
    GpuRenderPass &operator=(GpuRenderPass &&rhs) = delete;

    /**
     * @return the attachments used to create this render pass.
     */
    auto getAttachments() const noexcept
        -> std::span<GpuAttachmentDescription const>;

    /**
     * @return the subpasses used to create this render pass.
     */
    auto getSubpasses() const noexcept
        -> std::span<GpuSubpassDescription const>;

    /**
     * @return the dependencies used to create this render pass.
     */
    auto getDependencies() const noexcept
        -> std::span<GpuSubpassDependency const>;

    /**
     * @return the underlying vk::RenderPass.
     */
    auto get() const noexcept -> vk::RenderPass;

  private:
    GpuRenderPassCreateInfo createInfo_;
    vk::UniqueRenderPass handle_;
  };

  /**
   * A class that creates and caches render passes.
   */
  export class GpuRenderPassCache {
  public:
    /**
     * @param device the device to be used by this cache to create render
     * passes.
     */
    explicit GpuRenderPassCache(vk::Device device);

    /**
     * If this function is called with createInfo equal to the createInfo of a
     * previous invocation, it returns the same render pass as the first
     * invocation. Otherwise, this function creates and returns a new render
     * pass.
     * @param createInfo a description of the render pass.
     * @return a render pass described by createInfo.
     */
    auto create(GpuRenderPassCreateInfo const &createInfo)
        -> GpuRenderPass const &;

  private:
    vk::Device device_;
    std::forward_list<std::vector<GpuAttachmentDescription>>
        attachmentDescriptions_;
    std::forward_list<std::vector<GpuAttachmentReference>>
        attachmentReferences_;
    std::forward_list<std::vector<std::uint32_t>> preserveAttachments_;
    std::forward_list<std::vector<GpuSubpassDescription>> subpassDescriptions_;
    std::forward_list<std::vector<GpuSubpassDependency>> subpassDependencies_;
    std::unordered_map<
        GpuRenderPassCreateInfo,
        GpuRenderPass,
        boost::hash<GpuRenderPassCreateInfo>>
        renderPasses_;
    std::mutex mutex_;
  };

  export struct GpuDescriptorSetLayoutBinding {
    vk::DescriptorType descriptorType;
    std::uint32_t descriptorCount;
    vk::ShaderStageFlags stageFlags;

    auto operator==(GpuDescriptorSetLayoutBinding const &rhs) const
        -> bool = default;
  };

  export auto hash_value(GpuDescriptorSetLayoutBinding const &binding) noexcept
      -> std::size_t;

  export struct GpuDescriptorSetLayoutCreateInfo {
    std::span<GpuDescriptorSetLayoutBinding const> bindings;
  };

  export auto operator==(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept -> bool;

  export auto operator!=(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept -> bool;

  export auto
  hash_value(GpuDescriptorSetLayoutCreateInfo const &createInfo) noexcept
      -> std::size_t;

  /**
   * A class that creates and caches descriptor set layouts.
   */
  export class GpuDescriptorSetLayoutCache {
  public:
    /**
     * @param device the device to be used by this cache to create descriptor
     * set layouts.
     */
    explicit GpuDescriptorSetLayoutCache(vk::Device device);

    /**
     * If this function is called with createInfo equal to the createInfo of a
     * previous invocation, it returns the same descriptor set layout as the
     * first invocation. Otherwise, this function creates and returns a new
     * descriptor set layout.
     * @param createInfo a description of the descriptor set layout.
     * @return a descriptor set layout described by createInfo.
     */
    vk::DescriptorSetLayout
    create(GpuDescriptorSetLayoutCreateInfo const &createInfo);

  private:
    vk::Device device_;
    std::forward_list<std::vector<GpuDescriptorSetLayoutBinding>> bindings_;
    std::unordered_map<
        GpuDescriptorSetLayoutCreateInfo,
        vk::UniqueDescriptorSetLayout,
        boost::hash<GpuDescriptorSetLayoutCreateInfo>>
        descriptorSetLayouts_;
    std::mutex mutex_;
  };

  export struct GpuPushConstantRange {
    vk::ShaderStageFlags stageFlags;
    std::uint32_t offset;
    std::uint32_t size;

    explicit operator vk::PushConstantRange() const noexcept {
      return vk::PushConstantRange{stageFlags, offset, size};
    }

    auto operator==(GpuPushConstantRange const &rhs) const -> bool = default;
  };

  export auto hash_value(GpuPushConstantRange const &range) noexcept
      -> std::size_t;

  export struct GpuPipelineLayoutCreateInfo {
    std::span<vk::DescriptorSetLayout const> setLayouts;
    std::span<GpuPushConstantRange const> pushConstantRanges;
  };

  export auto operator==(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept -> bool;

  export auto operator!=(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept -> bool;

  export auto hash_value(GpuPipelineLayoutCreateInfo const &createInfo) noexcept
      -> std::size_t;
} // namespace mobula