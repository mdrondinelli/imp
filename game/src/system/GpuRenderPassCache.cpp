#include "GpuRenderPassCache.h"

#include <gsl-lite/gsl-lite.hpp>

namespace imp {
  GpuRenderPassCache::GpuRenderPassCache(vk::Device device): device_{device} {}

  vk::RenderPass
  GpuRenderPassCache::create(GpuRenderPassCreateInfo const &createInfo) {
    auto lock = std::scoped_lock{mutex_};
    if (auto it = renderPasses_.find(createInfo); it != renderPasses_.end()) {
      return *it->second;
    }
    auto ownedAttachmentDescriptions = std::vector(
        createInfo.attachments.begin(), createInfo.attachments.end());
    auto vulkanAttachmentDescriptions =
        std::vector<vk::AttachmentDescription>{};
    vulkanAttachmentDescriptions.reserve(createInfo.attachments.size());
    for (auto &attachmentDescription : createInfo.attachments) {
      vulkanAttachmentDescriptions.emplace_back(
          static_cast<vk::AttachmentDescription>(attachmentDescription));
    }
    auto attachmentReferenceCount = std::size_t{};
    auto preserveAttachmentCount = std::size_t{};
    for (auto &subpass : createInfo.subpasses) {
      attachmentReferenceCount += subpass.inputAttachments.size();
      attachmentReferenceCount += subpass.colorAttachments.size();
      gsl_Expects(
          subpass.resolveAttachments.size() ==
              subpass.colorAttachments.size() ||
          subpass.resolveAttachments.empty());
      attachmentReferenceCount += subpass.resolveAttachments.size();
      attachmentReferenceCount += subpass.depthStencilAttachment != nullptr;
      preserveAttachmentCount += subpass.preserveAttachments.size();
    }
    auto ownedAttachmentReferences = std::vector<GpuAttachmentReference>{};
    auto ownedPreserveAttachments = std::vector<std::uint32_t>{};
    auto vulkanAttachmentReferences = std::vector<vk::AttachmentReference>{};
    ownedAttachmentReferences.reserve(attachmentReferenceCount);
    ownedPreserveAttachments.reserve(preserveAttachmentCount);
    vulkanAttachmentReferences.reserve(attachmentReferenceCount);
    auto ownedSubpassDescriptions = std::vector<GpuSubpassDescription>{};
    auto vulkanSubpassDescriptions = std::vector<vk::SubpassDescription>{};
    ownedSubpassDescriptions.reserve(createInfo.subpasses.size());
    vulkanSubpassDescriptions.reserve(createInfo.subpasses.size());
    for (auto &subpass : createInfo.subpasses) {
      auto &ownedSubpass = ownedSubpassDescriptions.emplace_back();
      auto &vulkanSubpass = vulkanSubpassDescriptions.emplace_back();
      ownedSubpass.pipelineBindPoint = subpass.pipelineBindPoint;
      vulkanSubpass.pipelineBindPoint = subpass.pipelineBindPoint;
      if (!subpass.inputAttachments.empty()) {
        ownedSubpass.inputAttachments = {
            ownedAttachmentReferences.data() + ownedAttachmentReferences.size(),
            subpass.inputAttachments.size()};
        vulkanSubpass.inputAttachmentCount =
            static_cast<std::uint32_t>(subpass.inputAttachments.size());
        vulkanSubpass.pInputAttachments = vulkanAttachmentReferences.data() +
                                          vulkanAttachmentReferences.size();
        ownedAttachmentReferences.insert(
            ownedAttachmentReferences.end(),
            subpass.inputAttachments.begin(),
            subpass.inputAttachments.end());
        for (auto &attachment : subpass.inputAttachments) {
          vulkanAttachmentReferences.emplace_back(
              static_cast<vk::AttachmentReference>(attachment));
        }
      }
      if (!subpass.colorAttachments.empty()) {
        ownedSubpass.colorAttachments = {
            ownedAttachmentReferences.data() + ownedAttachmentReferences.size(),
            subpass.colorAttachments.size()};
        vulkanSubpass.colorAttachmentCount =
            static_cast<std::uint32_t>(subpass.colorAttachments.size());
        vulkanSubpass.pColorAttachments = vulkanAttachmentReferences.data() +
                                          vulkanAttachmentReferences.size();
        ownedAttachmentReferences.insert(
            ownedAttachmentReferences.end(),
            subpass.colorAttachments.begin(),
            subpass.colorAttachments.end());
        for (auto &attachment : subpass.colorAttachments) {
          vulkanAttachmentReferences.emplace_back(
              static_cast<vk::AttachmentReference>(attachment));
        }
        if (!subpass.resolveAttachments.empty()) {
          ownedSubpass.resolveAttachments = {
              ownedAttachmentReferences.data() +
                  ownedAttachmentReferences.size(),
              subpass.resolveAttachments.size()};
          ownedAttachmentReferences.insert(
              ownedAttachmentReferences.end(),
              subpass.resolveAttachments.begin(),
              subpass.resolveAttachments.end());
          vulkanSubpass.pResolveAttachments =
              vulkanAttachmentReferences.data() +
              vulkanAttachmentReferences.size();
          for (auto &attachment : subpass.resolveAttachments) {
            vulkanAttachmentReferences.emplace_back(
                static_cast<vk::AttachmentReference>(attachment));
          }
        }
      }
      if (subpass.depthStencilAttachment) {
        ownedSubpass.depthStencilAttachment =
            ownedAttachmentReferences.data() + ownedAttachmentReferences.size();
        vulkanSubpass.pDepthStencilAttachment =
            vulkanAttachmentReferences.data() +
            vulkanAttachmentReferences.size();
        ownedAttachmentReferences.emplace_back(*subpass.depthStencilAttachment);
        vulkanAttachmentReferences.emplace_back(
            static_cast<vk::AttachmentReference>(
                *subpass.depthStencilAttachment));
      }
      if (!subpass.preserveAttachments.empty()) {
        ownedSubpass.preserveAttachments = {
            ownedPreserveAttachments.data() + ownedPreserveAttachments.size(),
            subpass.preserveAttachments.size()};
        vulkanSubpass.preserveAttachmentCount =
            static_cast<std::uint32_t>(subpass.preserveAttachments.size());
        vulkanSubpass.pPreserveAttachments =
            ownedPreserveAttachments.data() + ownedPreserveAttachments.size();
        ownedPreserveAttachments.insert(
            ownedPreserveAttachments.end(),
            subpass.preserveAttachments.begin(),
            subpass.preserveAttachments.end());
      }
    }
    auto ownedSubpassDependencies = std::vector(
        createInfo.dependencies.begin(), createInfo.dependencies.end());
    auto vulkanSubpassDependencies = std::vector<vk::SubpassDependency>{};
    vulkanSubpassDependencies.reserve(createInfo.dependencies.size());
    for (auto &dependency : createInfo.dependencies) {
      vulkanSubpassDependencies.emplace_back(
          static_cast<vk::SubpassDependency>(dependency));
    }
    auto ownedCreateInfo = GpuRenderPassCreateInfo{};
    ownedCreateInfo.attachments = ownedAttachmentDescriptions;
    ownedCreateInfo.subpasses = ownedSubpassDescriptions;
    ownedCreateInfo.dependencies = ownedSubpassDependencies;
    auto vulkanCreateInfo = vk::RenderPassCreateInfo{};
    vulkanCreateInfo.attachmentCount =
        static_cast<std::uint32_t>(vulkanAttachmentDescriptions.size());
    vulkanCreateInfo.pAttachments = vulkanAttachmentDescriptions.data();
    vulkanCreateInfo.subpassCount =
        static_cast<std::uint32_t>(vulkanSubpassDescriptions.size());
    vulkanCreateInfo.pSubpasses = vulkanSubpassDescriptions.data();
    vulkanCreateInfo.dependencyCount =
        static_cast<std::uint32_t>(vulkanSubpassDependencies.size());
    vulkanCreateInfo.pDependencies = vulkanSubpassDependencies.data();
    attachmentDescriptions_.emplace_front(
        std::move(ownedAttachmentDescriptions));
    attachmentReferences_.emplace_front(std::move(ownedAttachmentReferences));
    preserveAttachments_.emplace_front(std::move(ownedPreserveAttachments));
    subpassDescriptions_.emplace_front(std::move(ownedSubpassDescriptions));
    subpassDependencies_.emplace_front(std::move(ownedSubpassDependencies));
    return *renderPasses_
                .emplace(
                    ownedCreateInfo,
                    device_.createRenderPassUnique(vulkanCreateInfo))
                .first->second;
  }
} // namespace imp