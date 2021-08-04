#include "GpuSamplerCache.h"

namespace imp {
  GpuSamplerCache::GpuSamplerCache(vk::Device device): device_{device} {}

  vk::Sampler GpuSamplerCache::create(GpuSamplerCreateInfo const &createInfo) {
    auto lock = std::scoped_lock{mutex_};
    if (auto it = samplers_.find(createInfo); it != samplers_.end()) {
      return *it->second;
    }
    return *samplers_
                .emplace(
                    createInfo,
                    device_.createSamplerUnique(
                        static_cast<vk::SamplerCreateInfo>(createInfo)))
                .first->second;
  }
} // namespace imp