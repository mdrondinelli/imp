#include "GpuSamplerCache.h"

#include "GpuContext.h"

namespace imp {
  GpuSamplerCache::GpuSamplerCache(GpuContext &context): context_{&context} {}

  vk::Sampler GpuSamplerCache::create(GpuSamplerCreateInfo const& createInfo) {
    auto it = samplers_.find(createInfo);
    if (it != samplers_.end()) {
      return *it->second;
    }
    auto vkCreateInfo = vk::SamplerCreateInfo{};
    vkCreateInfo.magFilter = createInfo.magFilter;
    vkCreateInfo.minFilter = createInfo.minFilter;
    vkCreateInfo.mipmapMode = createInfo.mipmapMode;
    vkCreateInfo.addressModeU = createInfo.addressModeU;
    vkCreateInfo.addressModeV = createInfo.addressModeV;
    vkCreateInfo.addressModeW = createInfo.addressModeW;
    vkCreateInfo.mipLodBias = createInfo.mipLodBias;
    vkCreateInfo.anisotropyEnable = createInfo.anisotropyEnable;
    vkCreateInfo.maxAnisotropy = createInfo.maxAnisotropy;
    vkCreateInfo.compareEnable = createInfo.compareEnable;
    vkCreateInfo.compareOp = createInfo.compareOp;
    vkCreateInfo.minLod = createInfo.minLod;
    vkCreateInfo.maxLod = createInfo.maxLod;
    vkCreateInfo.borderColor = createInfo.borderColor;
    return *samplers_.emplace(
        createInfo, context_->getDevice().createSamplerUnique(vkCreateInfo)).first->second;
  }
}