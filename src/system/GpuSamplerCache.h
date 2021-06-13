#pragma once

#include <absl/container/flat_hash_map.h>
#include <vulkan/vulkan.hpp>

namespace imp {
  class GpuContext;

  struct GpuSamplerCreateInfo {
    vk::Filter magFilter;
    vk::Filter minFilter;
    vk::SamplerMipmapMode mipmapMode;
    vk::SamplerAddressMode addressModeU;
    vk::SamplerAddressMode addressModeV;
    vk::SamplerAddressMode addressModeW;
    float mipLodBias;
    bool anisotropyEnable;
    float maxAnisotropy;
    bool compareEnable;
    vk::CompareOp compareOp;
    float minLod;
    float maxLod;
    vk::BorderColor borderColor;
  };

  inline bool operator==(
      GpuSamplerCreateInfo const &lhs,
      GpuSamplerCreateInfo const &rhs) noexcept {
    return lhs.magFilter == rhs.magFilter && lhs.minFilter == rhs.minFilter &&
           lhs.mipmapMode == rhs.mipmapMode &&
           lhs.addressModeU == rhs.addressModeU &&
           lhs.addressModeV == rhs.addressModeV &&
           lhs.addressModeW == rhs.addressModeW &&
           lhs.mipLodBias == rhs.mipLodBias &&
           lhs.anisotropyEnable == rhs.anisotropyEnable &&
           lhs.maxAnisotropy == rhs.maxAnisotropy &&
           lhs.compareEnable == rhs.compareEnable &&
           lhs.compareOp == rhs.compareOp && lhs.minLod == rhs.minLod &&
           lhs.maxLod == rhs.maxLod && lhs.borderColor == rhs.borderColor;
  }

  inline bool operator!=(
      GpuSamplerCreateInfo const &lhs,
      GpuSamplerCreateInfo const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<typename H>
  H AbslHashValue(H state, GpuSamplerCreateInfo const &createInfo) noexcept {
    return H::combine(
        std::move(state),
        createInfo.magFilter,
        createInfo.minFilter,
        createInfo.mipmapMode,
        createInfo.addressModeU,
        createInfo.addressModeV,
        createInfo.addressModeW,
        createInfo.mipLodBias,
        createInfo.anisotropyEnable,
        createInfo.maxAnisotropy,
        createInfo.compareEnable,
        createInfo.compareOp,
        createInfo.minLod,
        createInfo.maxLod,
        createInfo.borderColor);
  }

  class GpuSamplerCache {
  public:
    explicit GpuSamplerCache(GpuContext &context);

    vk::Sampler create(GpuSamplerCreateInfo const &createInfo);

  private:
    GpuContext *context_;
    absl::flat_hash_map<GpuSamplerCreateInfo, vk::UniqueSampler> samplers_;
  };
} // namespace imp