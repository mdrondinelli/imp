#pragma once

#include <forward_list>
#include <memory>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace imp {
  class GpuContext;

  struct GpuDescriptorSetLayoutBinding {
    vk::DescriptorType descriptorType;
    std::uint32_t descriptorCount;
    vk::ShaderStageFlags stageFlags;
  };

  struct GpuDescriptorSetLayoutCreateInfo {
    std::uint32_t bindingCount;
    GpuDescriptorSetLayoutBinding const *bindings;
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
    if (lhs.bindingCount != rhs.bindingCount) {
      return false;
    }
    for (auto i = 0u; i < lhs.bindingCount; ++i) {
      if (lhs.bindings[i] != rhs.bindings[i]) {
        return false;
      }
    }
    return true;
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
    if (lhs.bindingCount != rhs.bindings.size()) {
      return false;
    }
    for (auto i = 0u; i < lhs.bindingCount; ++i) {
      if (lhs.bindings[i] != rhs.bindings[i]) {
        return false;
      }
    }
    return true;
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
    return !(lhs == rhs);
  }

  template<typename H>
  H AbslHashValue(
      H state, GpuDescriptorSetLayoutBinding const &binding) noexcept {
    return H::combine(
        std::move(state),
        binding.descriptorType,
        binding.descriptorCount,
        static_cast<vk::ShaderStageFlags::MaskType>(binding.stageFlags));
  }

  template<typename H>
  H AbslHashValue(
      H state, GpuDescriptorSetLayoutCreateInfo const &createInfo) noexcept {
    return H::combine_contiguous(
        std::move(state), createInfo.bindings, createInfo.bindingCount);
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