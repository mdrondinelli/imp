#pragma once

#include <algorithm>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "../util/Gsl.h"

namespace imp {
  struct GpuPushConstantRange {
    vk::ShaderStageFlags stageFlags;
    std::uint32_t offset;
    std::uint32_t size;
  };

  struct GpuPipelineLayoutCreateInfo {
    gsl::span<vk::DescriptorSetLayout const> setLayouts;
    gsl::span<GpuPushConstantRange const> pushConstantRanges;
  };

  struct GpuPipelineLayoutInfo {
    std::vector<vk::DescriptorSetLayout> setLayouts;
    std::vector<GpuPushConstantRange> pushConstantRanges;
  };

  inline bool operator==(
      GpuPushConstantRange const &lhs,
      GpuPushConstantRange const &rhs) noexcept {
    return lhs.stageFlags == rhs.stageFlags && lhs.offset == rhs.offset &&
           lhs.size == rhs.size;
  }

  inline bool operator!=(
      GpuPushConstantRange const &lhs,
      GpuPushConstantRange const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline bool operator==(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept {
    return lhs.setLayouts.size() == rhs.setLayouts.size() &&
           lhs.pushConstantRanges.size() == rhs.pushConstantRanges.size() &&
           (lhs.setLayouts.data() == rhs.setLayouts.data() ||
            std::equal(
                lhs.setLayouts.begin(),
                lhs.setLayouts.end(),
                rhs.setLayouts.begin())) &&
           (lhs.pushConstantRanges.data() == rhs.pushConstantRanges.data() ||
            std::equal(
                lhs.pushConstantRanges.begin(),
                lhs.pushConstantRanges.end(),
                rhs.pushConstantRanges.begin()));
  }

  inline bool operator!=(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline bool operator==(
      GpuPipelineLayoutInfo const &lhs,
      GpuPipelineLayoutInfo const &rhs) noexcept {
    return lhs.setLayouts == rhs.setLayouts &&
           lhs.pushConstantRanges == rhs.pushConstantRanges;
  }

  inline bool operator!=(
      GpuPipelineLayoutInfo const &lhs,
      GpuPipelineLayoutInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline bool operator==(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutInfo const &rhs) noexcept {
    return lhs.setLayouts.size() == rhs.setLayouts.size() &&
           lhs.pushConstantRanges.size() == rhs.pushConstantRanges.size() &&
           (lhs.setLayouts.data() == rhs.setLayouts.data() ||
            std::equal(
                lhs.setLayouts.begin(),
                lhs.setLayouts.end(),
                rhs.setLayouts.begin())) &&
           (lhs.pushConstantRanges.data() == rhs.pushConstantRanges.data() ||
            std::equal(
                lhs.pushConstantRanges.begin(),
                lhs.pushConstantRanges.end(),
                rhs.pushConstantRanges.begin()));
  }

  inline bool operator!=(
      GpuPipelineLayoutCreateInfo const &lhs,
      GpuPipelineLayoutInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline bool operator==(
      GpuPipelineLayoutInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept {
    return rhs == lhs;
  }

  inline bool operator!=(
      GpuPipelineLayoutInfo const &lhs,
      GpuPipelineLayoutCreateInfo const &rhs) noexcept {
    return !(rhs == lhs);
  }

  template<typename H>
  H AbslHashValue(H state, GpuPushConstantRange const &range) noexcept {
    return H::combine(
        std::move(state),
        static_cast<VkShaderStageFlags>(range.stageFlags),
        range.offset,
        range.size);
  }

  template<typename H>
  H AbslHashValue(H state, GpuPipelineLayoutCreateInfo const &info) noexcept {
    state = H::combine_contiguous(
        std::move(state), info.setLayouts.data(), info.setLayouts.size());
    state = H::combine_contiguous(
        std::move(state),
        info.pushConstantRanges.data(),
        info.pushConstantRanges.size());
    return state;
  }

  template<typename H>
  H AbslHashValue(H state, GpuPipelineLayoutInfo const &info) noexcept {
    state = H::combine_contiguous(
        std::move(state), info.setLayouts.data(), info.setLayouts.size());
    state = H::combine_contiguous(
        std::move(state),
        info.pushConstantRanges.data(),
        info.pushConstantRanges.size());
    return state;
  }

  class GpuContext;

  class GpuPipelineLayoutCache {
  public:
    explicit GpuPipelineLayoutCache(gsl::not_null<GpuContext *> context);

    vk::PipelineLayout create(GpuPipelineLayoutCreateInfo const &createInfo);

  private:
    struct Node {
      std::unique_ptr<Node> next;
      GpuPipelineLayoutInfo key;
      vk::UniquePipelineLayout value;
    };

    gsl::not_null<GpuContext *> context_;
    std::vector<std::unique_ptr<Node>> buckets_;
    std::size_t size_;
  };
} // namespace imp