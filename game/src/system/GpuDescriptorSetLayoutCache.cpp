#include "GpuDescriptorSetLayoutCache.h"

namespace imp {
  GpuDescriptorSetLayoutCache::GpuDescriptorSetLayoutCache(vk::Device device):
      device_{device} {}

  vk::DescriptorSetLayout GpuDescriptorSetLayoutCache::create(
      GpuDescriptorSetLayoutCreateInfo const &createInfo) {
    auto lock = std::scoped_lock{mutex_};
    if (auto it = descriptorSetLayouts_.find(createInfo);
        it != descriptorSetLayouts_.end()) {
      return *it->second;
    }
    auto ownedBindings = std::vector<GpuDescriptorSetLayoutBinding>(
        createInfo.bindings.begin(), createInfo.bindings.end());
    auto vulkanBindings = std::vector<vk::DescriptorSetLayoutBinding>{};
    vulkanBindings.resize(createInfo.bindings.size());
    for (auto i = std::size_t{}; i < ownedBindings.size(); ++i) {
      vulkanBindings[i].binding = static_cast<std::uint32_t>(i);
      vulkanBindings[i].descriptorType = ownedBindings[i].descriptorType;
      vulkanBindings[i].descriptorCount = ownedBindings[i].descriptorCount;
      vulkanBindings[i].stageFlags = ownedBindings[i].stageFlags;
    }
    auto ownedCreateInfo = GpuDescriptorSetLayoutCreateInfo{};
    ownedCreateInfo.bindings = ownedBindings;
    auto vulkanCreateInfo = vk::DescriptorSetLayoutCreateInfo{};
    vulkanCreateInfo.bindingCount =
        static_cast<std::uint32_t>(vulkanBindings.size());
    vulkanCreateInfo.pBindings = vulkanBindings.data();
    bindings_.emplace_front(std::move(ownedBindings));
    return *descriptorSetLayouts_
                .emplace(
                    ownedCreateInfo,
                    device_.createDescriptorSetLayoutUnique(vulkanCreateInfo))
                .first->second;
  }
} // namespace imp