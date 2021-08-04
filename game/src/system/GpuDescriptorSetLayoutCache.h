#pragma once

#include <algorithm>
#include <list>
#include <mutex>
#include <span>
#include <unordered_map>
#include <vector>

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

namespace imp {
  struct GpuDescriptorSetLayoutBinding {
    vk::DescriptorType descriptorType;
    std::uint32_t descriptorCount;
    vk::ShaderStageFlags stageFlags;
  };

  inline bool operator==(
      GpuDescriptorSetLayoutBinding const &lhs,
      GpuDescriptorSetLayoutBinding const &rhs) noexcept {
    return lhs.descriptorType == rhs.descriptorType &&
           lhs.descriptorCount == rhs.descriptorCount &&
           lhs.stageFlags == rhs.stageFlags;
  }

  inline bool operator!=(
      GpuDescriptorSetLayoutBinding const &lhs,
      GpuDescriptorSetLayoutBinding const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline std::size_t
  hash_value(GpuDescriptorSetLayoutBinding const &binding) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, binding.descriptorType);
    boost::hash_combine(seed, binding.descriptorCount);
    boost::hash_combine(
        seed, static_cast<VkShaderStageFlags>(binding.stageFlags));
    return seed;
  }

  struct GpuDescriptorSetLayoutCreateInfo {
    std::span<GpuDescriptorSetLayoutBinding const> bindings;
  };

  inline bool operator==(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept {
    return lhs.bindings.size() == rhs.bindings.size() &&
           (lhs.bindings.data() == rhs.bindings.data() ||
            std::ranges::equal(lhs.bindings, rhs.bindings));
  }

  inline bool operator!=(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline std::size_t
  hash_value(GpuDescriptorSetLayoutCreateInfo const &createInfo) noexcept {
    return boost::hash_range(
        createInfo.bindings.begin(), createInfo.bindings.end());
  }

  class GpuDescriptorSetLayoutCache {
  public:
    explicit GpuDescriptorSetLayoutCache(vk::Device device);

    vk::DescriptorSetLayout
    create(GpuDescriptorSetLayoutCreateInfo const &createInfo);

  private:
    vk::Device device_;
    std::list<std::vector<GpuDescriptorSetLayoutBinding>> bindings_;
    std::unordered_map<
        GpuDescriptorSetLayoutCreateInfo,
        vk::UniqueDescriptorSetLayout,
        boost::hash<GpuDescriptorSetLayoutCreateInfo>>
        descriptorSetLayouts_;
    std::mutex mutex_;
  };
} // namespace imp