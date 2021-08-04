#pragma once

#include <algorithm>
#include <list>
#include <mutex>
#include <span>
#include <vector>

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

namespace imp {
  struct GpuPushConstantRange {
    vk::ShaderStageFlags stageFlags;
    std::uint32_t offset;
    std::uint32_t size;

    friend bool operator==(
        GpuPushConstantRange const &lhs,
        GpuPushConstantRange const &rhs) = default;
  };

  inline std::size_t hash_value(GpuPushConstantRange const &range) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, static_cast<VkFlags>(range.stageFlags));
    boost::hash_combine(seed, range.offset);
    boost::hash_combine(seed, range.size);
    return seed;
  }

  struct GpuPipelineLayoutCreateInfo {
    std::span<vk::DescriptorSetLayout const> setLayouts;
    std::span<GpuPushConstantRange const> pushConstantRanges;
  };

  inline bool operator==(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept {
    return (lhs.setLayouts.data() == rhs.setLayouts.data() &&
                lhs.setLayouts.size() == rhs.setLayouts.size() ||
            std::ranges::equal(lhs.setLayouts, rhs.setLayouts)) &&
           (lhs.pushConstantRanges.data() == rhs.pushConstantRanges.data() &&
                lhs.pushConstantRanges.size() ==
                    rhs.pushConstantRanges.size() ||
            std::ranges::equal(lhs.pushConstantRanges, rhs.pushConstantRanges));
  }

  inline bool operator!=(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline std::size_t
  hash_value(GpuPipelineLayoutCreateInfo const &createInfo) noexcept {
    auto seed = std::size_t{};
    auto setLayoutsBegin = reinterpret_cast<VkDescriptorSetLayout const *>(
        createInfo.setLayouts.data());
    auto setLayoutsEnd = setLayoutsBegin + createInfo.setLayouts.size();
    boost::hash_range(seed, setLayoutsBegin, setLayoutsEnd);
    boost::hash_range(
        seed,
        createInfo.pushConstantRanges.begin(),
        createInfo.pushConstantRanges.end());
    return seed;
  }

  class GpuPipelineLayoutCache {
  public:
    explicit GpuPipelineLayoutCache(vk::Device device);

    vk::PipelineLayout create(GpuPipelineLayoutCreateInfo const &createInfo);

  private:
    vk::Device device_;
    std::list<std::vector<vk::DescriptorSetLayout>> setLayouts_;
    std::list<std::vector<GpuPushConstantRange>> pushConstantRanges_;
    std::unordered_map<
        GpuPipelineLayoutCreateInfo,
        vk::UniquePipelineLayout,
        boost::hash<GpuPipelineLayoutCreateInfo>>
        pipelineLayouts_;
    std::mutex mutex_;
  };
} // namespace imp