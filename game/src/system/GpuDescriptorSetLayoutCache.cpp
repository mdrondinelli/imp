#include "GpuDescriptorSetLayoutCache.h"

#include <absl/hash/hash.h>

#include "GpuContext.h"

namespace imp {
  namespace {
    auto hash(GpuDescriptorSetLayoutCreateInfo const &createInfo) noexcept {
      return absl::Hash<GpuDescriptorSetLayoutCreateInfo>{}(createInfo);
    }

    auto hash(GpuDescriptorSetLayoutInfo const &info) noexcept {
      return absl::Hash<GpuDescriptorSetLayoutInfo>{}(info);
    }
  } // namespace

  GpuDescriptorSetLayoutCache::GpuDescriptorSetLayoutCache(GpuContext &context):
      context_{&context}, size_{0}, buckets_(16) {}

  vk::DescriptorSetLayout GpuDescriptorSetLayoutCache::create(
      GpuDescriptorSetLayoutCreateInfo const &createInfo) {
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
    auto vkBindings = std::vector<vk::DescriptorSetLayoutBinding>{};
    vkBindings.resize(createInfo.bindings.size());
    for (auto i = std::size_t{}; i < createInfo.bindings.size(); ++i) {
      auto &binding = createInfo.bindings[i];
      auto &vkBinding = vkBindings[i];
      vkBinding.binding = i;
      vkBinding.descriptorType = binding.descriptorType;
      vkBinding.descriptorCount = binding.descriptorCount;
      vkBinding.stageFlags = binding.stageFlags;
    }
    auto vkCreateInfo = vk::DescriptorSetLayoutCreateInfo{};
    vkCreateInfo.bindingCount = static_cast<std::uint32_t>(vkBindings.size());
    vkCreateInfo.pBindings = vkBindings.data();
    auto node = std::make_unique<Node>();
    node->next = std::move(buckets_[index]);
    node->key.bindings.assign(
        createInfo.bindings.begin(), createInfo.bindings.end());
    node->value =
        context_->getDevice().createDescriptorSetLayoutUnique(vkCreateInfo);
    buckets_[index] = std::move(node);
    return *buckets_[index]->value;
  }
} // namespace imp