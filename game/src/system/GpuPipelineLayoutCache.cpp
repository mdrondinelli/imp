#include "GpuPipelineLayoutCache.h"

namespace imp {
  GpuPipelineLayoutCache::GpuPipelineLayoutCache(vk::Device device):
      device_{device} {}

  vk::PipelineLayout GpuPipelineLayoutCache::create(
      GpuPipelineLayoutCreateInfo const &createInfo) {
    auto lock = std::scoped_lock{mutex_};
    if (auto it = pipelineLayouts_.find(createInfo);
        it != pipelineLayouts_.end()) {
      return *it->second;
    }
    auto ownedSetLayouts =
        std::vector(createInfo.setLayouts.begin(), createInfo.setLayouts.end());
    auto ownedPushConstantRanges = std::vector(
        createInfo.pushConstantRanges.begin(),
        createInfo.pushConstantRanges.end());
    auto ownedCreateInfo = GpuPipelineLayoutCreateInfo{};
    ownedCreateInfo.setLayouts = ownedSetLayouts;
    ownedCreateInfo.pushConstantRanges = ownedPushConstantRanges;
    auto vulkanPushConstantRanges = std::vector<vk::PushConstantRange>{};
    vulkanPushConstantRanges.reserve(ownedPushConstantRanges.size());
    for (auto &pushConstantRange : ownedPushConstantRanges) {
      vulkanPushConstantRanges.emplace_back(
          pushConstantRange.stageFlags,
          pushConstantRange.offset,
          pushConstantRange.size);
    }
    auto vulkanCreateInfo = vk::PipelineLayoutCreateInfo{};
    vulkanCreateInfo.setLayoutCount =
        static_cast<std::uint32_t>(ownedSetLayouts.size());
    vulkanCreateInfo.pSetLayouts = ownedSetLayouts.data();
    vulkanCreateInfo.pushConstantRangeCount =
        static_cast<std::uint32_t>(vulkanPushConstantRanges.size());
    vulkanCreateInfo.pPushConstantRanges = vulkanPushConstantRanges.data();
    setLayouts_.emplace_front(std::move(ownedSetLayouts));
    pushConstantRanges_.emplace_front(std::move(ownedPushConstantRanges));
    return *pipelineLayouts_
                .emplace(
                    ownedCreateInfo,
                    device_.createPipelineLayoutUnique(vulkanCreateInfo))
                .first->second;
  }
} // namespace imp