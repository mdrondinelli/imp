import mobula.engine.gpu;

// clang-format off
import <span>;
// clang-format on

#include <vulkan/vulkan.hpp>

namespace mobula {
  GpuRenderPass::GpuRenderPass(
      GpuRenderPassCreateInfo const &createInfo,
      vk::UniqueRenderPass handle) noexcept:
      createInfo_{createInfo}, handle_{std::move(handle)} {}

  auto GpuRenderPass::getAttachments() const noexcept
      -> std::span<GpuAttachmentDescription const> {
    return createInfo_.attachments;
  }

  auto GpuRenderPass::getSubpasses() const noexcept
      -> std::span<GpuSubpassDescription const> {
    return createInfo_.subpasses;
  }

  auto GpuRenderPass::getDependencies() const noexcept
      -> std::span<GpuSubpassDependency const> {
    return createInfo_.dependencies;
  }

  auto GpuRenderPass::get() const noexcept -> vk::RenderPass {
    return *handle_;
  }
} // namespace mobula