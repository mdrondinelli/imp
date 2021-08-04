#pragma once

#include <mutex>
#include <unordered_map>

#include <boost/container_hash/hash.hpp>
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

    explicit operator vk::SamplerCreateInfo() const noexcept {
      return vk::SamplerCreateInfo{
          {},
          magFilter,
          minFilter,
          mipmapMode,
          addressModeU,
          addressModeV,
          addressModeW,
          mipLodBias,
          anisotropyEnable,
          maxAnisotropy,
          compareEnable,
          compareOp,
          minLod,
          maxLod,
          borderColor};
    }

    friend bool operator==(
        GpuSamplerCreateInfo const &lhs,
        GpuSamplerCreateInfo const &rhs) = default;
  };

  inline std::size_t
  hash_value(GpuSamplerCreateInfo const &createInfo) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, createInfo.magFilter);
    boost::hash_combine(seed, createInfo.minFilter);
    boost::hash_combine(seed, createInfo.mipmapMode);
    boost::hash_combine(seed, createInfo.addressModeU);
    boost::hash_combine(seed, createInfo.addressModeV);
    boost::hash_combine(seed, createInfo.addressModeW);
    boost::hash_combine(seed, createInfo.mipLodBias);
    boost::hash_combine(seed, createInfo.anisotropyEnable);
    boost::hash_combine(seed, createInfo.maxAnisotropy);
    boost::hash_combine(seed, createInfo.compareEnable);
    boost::hash_combine(seed, createInfo.compareOp);
    boost::hash_combine(seed, createInfo.minLod);
    boost::hash_combine(seed, createInfo.maxLod);
    boost::hash_combine(seed, createInfo.borderColor);
    return seed;
  }

  class GpuSamplerCache {
  public:
    explicit GpuSamplerCache(vk::Device device);

    vk::Sampler create(GpuSamplerCreateInfo const &createInfo);

  private:
    vk::Device device_;
    std::unordered_map<
        GpuSamplerCreateInfo,
        vk::UniqueSampler,
        boost::hash<GpuSamplerCreateInfo>>
        samplers_;
    std::mutex mutex_;
  };
} // namespace imp