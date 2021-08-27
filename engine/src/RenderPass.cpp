// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.gpu;
import <forward_list>;
import <vector>;
// clang-format on

namespace mobula {
  namespace gpu {
    RenderPass::RenderPass(vk::Device device, RenderPassParams const &params):
        params_{params} {
      auto vkCreateInfo = vk::RenderPassCreateInfo{};
      auto vkAttachments = std::vector<vk::AttachmentDescription>{};
      if (!params.attachments.empty()) {
        vkAttachments.reserve(params.attachments.size());
        for (auto &attachment : params.attachments) {
          auto &vkAttachment = vkAttachments.emplace_back();
          vkAttachment.flags = attachment.flags;
          vkAttachment.format = attachment.format;
          vkAttachment.samples = vk::SampleCountFlagBits::e1;
          vkAttachment.loadOp = attachment.loadOp;
          vkAttachment.storeOp = attachment.storeOp;
          vkAttachment.stencilLoadOp = attachment.stencilLoadOp;
          vkAttachment.stencilStoreOp = attachment.stencilStoreOp;
          vkAttachment.initialLayout = attachment.initialLayout;
          vkAttachment.finalLayout = attachment.finalLayout;
        }
        vkCreateInfo.attachmentCount =
            static_cast<std::uint32_t>(vkAttachments.size());
        vkCreateInfo.pAttachments = vkAttachments.data();
      }
      auto vkSubpasses = std::vector<vk::SubpassDescription>{};
      auto vkSubpassAttachments =
          std::forward_list<std::vector<vk::AttachmentReference>>{};
      vkSubpasses.reserve(params.subpasses.size());
      for (auto &subpass : params.subpasses) {
        auto &vkSubpass = vkSubpasses.emplace_back();
        vkSubpass.pipelineBindPoint = subpass.pipelineBindPoint;
        auto vkInputAttachments = std::vector<vk::AttachmentReference>{};
        if (!subpass.inputAttachments.empty()) {
          vkInputAttachments.reserve(subpass.inputAttachments.size());
          for (auto &attachment : subpass.inputAttachments) {
            vkInputAttachments.emplace_back(
                attachment.attachment, attachment.layout);
          }
          vkSubpass.inputAttachmentCount =
              static_cast<std::uint32_t>(vkInputAttachments.size());
          vkSubpass.pInputAttachments = vkInputAttachments.data();
          vkSubpassAttachments.emplace_front(std::move(vkInputAttachments));
        }
        auto vkColorAttachments = std::vector<vk::AttachmentReference>{};
        if (!subpass.colorAttachments.empty()) {
          vkColorAttachments.reserve(subpass.colorAttachments.size());
          for (auto &attachment : subpass.colorAttachments) {
            vkColorAttachments.emplace_back(
                attachment.attachment, attachment.layout);
          }
          vkSubpass.colorAttachmentCount =
              static_cast<std::uint32_t>(vkColorAttachments.size());
          vkSubpass.pColorAttachments = vkColorAttachments.data();
          vkSubpassAttachments.emplace_front(std::move(vkColorAttachments));
        }
        auto vkDepthStencilAttachment = std::vector<vk::AttachmentReference>{};
        if (subpass.depthStencilAttachment) {
          vkDepthStencilAttachment.emplace_back(
              subpass.depthStencilAttachment->attachment,
              subpass.depthStencilAttachment->layout);
          vkSubpass.pDepthStencilAttachment = vkDepthStencilAttachment.data();
          vkSubpassAttachments.emplace_front(
              std::move(vkDepthStencilAttachment));
        }
        if (!subpass.preserveAttachments.empty()) {
          vkSubpass.preserveAttachmentCount =
              static_cast<std::uint32_t>(subpass.preserveAttachments.size());
          vkSubpass.pPreserveAttachments = subpass.preserveAttachments.data();
        }
      }
      vkCreateInfo.subpassCount =
          static_cast<std::uint32_t>(vkSubpasses.size());
      vkCreateInfo.pSubpasses = vkSubpasses.data();
      auto vkDependencies = std::vector<vk::SubpassDependency>{};
      if (!params.dependencies.empty()) {
        vkDependencies.reserve(params.dependencies.size());
        for (auto &dependency : params.dependencies) {
          auto &vkDependency = vkDependencies.emplace_back();
          vkDependency.srcSubpass = dependency.srcSubpass;
          vkDependency.dstSubpass = dependency.dstSubpass;
          vkDependency.srcStageMask = dependency.srcStageMask;
          vkDependency.dstStageMask = dependency.dstStageMask;
          vkDependency.srcAccessMask = dependency.srcAccessMask;
          vkDependency.dstAccessMask = dependency.dstAccessMask;
          vkDependency.dependencyFlags = dependency.flags;
        }
        vkCreateInfo.dependencyCount =
            static_cast<std::uint32_t>(vkDependencies.size());
        vkCreateInfo.pDependencies = vkDependencies.data();
      }
      handle_ = device.createRenderPassUnique(vkCreateInfo);
    }
  } // namespace gpu
} // namespace mobula