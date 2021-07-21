#include "GpuPipelineLayoutCache.h"

#include <absl/hash/hash.h>

#include "GpuContext.h"

namespace imp {
  namespace {
    auto hash(GpuPipelineLayoutCreateInfo const &info) noexcept {
      return absl::Hash<GpuPipelineLayoutCreateInfo>{}(info);
    }

    auto hash(GpuPipelineLayoutInfo const &info) noexcept {
      return absl::Hash<GpuPipelineLayoutInfo>{}(info);
    }
  } // namespace

  GpuPipelineLayoutCache::GpuPipelineLayoutCache(
      gsl::not_null<GpuContext *> context):
      context_{context}, buckets_(16), size_{0} {}

  vk::PipelineLayout GpuPipelineLayoutCache::create(
      GpuPipelineLayoutCreateInfo const &createInfo) {
    auto hashValue = hash(createInfo);
    auto index = hashValue & (buckets_.size() - 1);
    for (auto node = buckets_[index].get(); node; node = node->next.get()) {
      if (node->key == createInfo) {
        return *node->value;
      }
    }
    ++size_;
    if (size_ > buckets_.size()) {
      auto buckets = std::vector<std::unique_ptr<Node>>(buckets_.size() * 2);
      for (auto &bucket : buckets_) {
        for (auto node = std::move(bucket); node;
             node = std::move(node->next)) {
          auto index = hash(node->key) & (buckets.size() - 1);
          node->next = std::move(buckets[index]);
          buckets[index] = std::move(node);
        }
      }
      buckets_ = std::move(buckets);
      index = hashValue & (buckets_.size() - 1);
    }
    auto vkPushConstantRanges = std::vector<vk::PushConstantRange>{};
    vkPushConstantRanges.reserve(createInfo.pushConstantRanges.size());
    for (auto &pushConstantRange : createInfo.pushConstantRanges) {
      vkPushConstantRanges.emplace_back(
          pushConstantRange.stageFlags,
          pushConstantRange.offset,
          pushConstantRange.size);
    }
    auto vkCreateInfo = vk::PipelineLayoutCreateInfo{};
    vkCreateInfo.setLayoutCount =
        static_cast<std::uint32_t>(createInfo.setLayouts.size());
    vkCreateInfo.pSetLayouts = createInfo.setLayouts.data();
    vkCreateInfo.pushConstantRangeCount =
        static_cast<std::uint32_t>(vkPushConstantRanges.size());
    vkCreateInfo.pPushConstantRanges = vkPushConstantRanges.data();
    auto node = std::make_unique<Node>();
    node->next = std::move(buckets_[index]);
    node->key.setLayouts.assign(
        createInfo.setLayouts.begin(), createInfo.setLayouts.end());
    node->key.pushConstantRanges.assign(
        createInfo.pushConstantRanges.begin(),
        createInfo.pushConstantRanges.end());
    node->value =
        context_->getDevice().createPipelineLayoutUnique(vkCreateInfo);
    buckets_[index] = std::move(node);
    return *buckets_[index]->value;
  }

} // namespace imp