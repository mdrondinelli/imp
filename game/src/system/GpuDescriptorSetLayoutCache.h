#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "../util/Gsl.h"

namespace imp {
  class GpuContext;

  struct GpuDescriptorSetLayoutBinding {
    vk::DescriptorType descriptorType;
    std::uint32_t descriptorCount;
    vk::ShaderStageFlags stageFlags;
  };

  struct GpuDescriptorSetLayoutCreateInfo {
    gsl::span<GpuDescriptorSetLayoutBinding const> bindings;
  };

  struct GpuDescriptorSetLayoutInfo {
    std::vector<GpuDescriptorSetLayoutBinding> bindings;
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

  inline bool operator==(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept {
    return lhs.bindings.size() == rhs.bindings.size() &&
           (lhs.bindings.data() == rhs.bindings.data() ||
            std::equal(
                lhs.bindings.begin(),
                lhs.bindings.end(),
                rhs.bindings.begin()));
  }

  inline bool operator!=(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline bool operator==(
      GpuDescriptorSetLayoutInfo const &lhs,
      GpuDescriptorSetLayoutInfo const &rhs) noexcept {
    return lhs.bindings == rhs.bindings;
  }

  inline bool operator!=(
      GpuDescriptorSetLayoutInfo const &lhs,
      GpuDescriptorSetLayoutInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline bool operator==(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutInfo const &rhs) noexcept {
    return lhs.bindings.size() == rhs.bindings.size() &&
           (lhs.bindings.data() == rhs.bindings.data() ||
            std::equal(
                lhs.bindings.begin(),
                lhs.bindings.end(),
                rhs.bindings.begin()));
  }

  inline bool operator!=(
      GpuDescriptorSetLayoutCreateInfo const &lhs,
      GpuDescriptorSetLayoutInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline bool operator==(
      GpuDescriptorSetLayoutInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept {
    return rhs == lhs;
  }

  inline bool operator!=(
      GpuDescriptorSetLayoutInfo const &lhs,
      GpuDescriptorSetLayoutCreateInfo const &rhs) noexcept {
    return !(rhs == lhs);
  }

  template<typename H>
  H AbslHashValue(
      H state, GpuDescriptorSetLayoutBinding const &binding) noexcept {
    return H::combine(
        std::move(state),
        binding.descriptorType,
        binding.descriptorCount,
        static_cast<VkShaderStageFlags>(binding.stageFlags));
  }

  template<typename H>
  H AbslHashValue(
      H state, GpuDescriptorSetLayoutCreateInfo const &createInfo) noexcept {
    return H::combine_contiguous(
        std::move(state),
        createInfo.bindings.data(),
        createInfo.bindings.size());
  }

  template<typename H>
  H AbslHashValue(H state, GpuDescriptorSetLayoutInfo const &info) noexcept {
    return H::combine_contiguous(
        std::move(state), info.bindings.data(), info.bindings.size());
  }

  class GpuDescriptorSetLayoutCache {
  public:
    explicit GpuDescriptorSetLayoutCache(GpuContext &context);

    vk::DescriptorSetLayout
    create(GpuDescriptorSetLayoutCreateInfo const &createInfo);

  private:
    struct Node {
      std::unique_ptr<Node> next;
      GpuDescriptorSetLayoutInfo key;
      vk::UniqueDescriptorSetLayout value;
    };

    GpuContext *context_;
    std::size_t size_;
    std::vector<std::unique_ptr<Node>> buckets_;
  };
} // namespace imp