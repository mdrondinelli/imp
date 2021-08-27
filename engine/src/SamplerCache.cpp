// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.gpu;
// clang-format on

namespace mobula {
  namespace gpu {
    SamplerCache::SamplerCache(vk::Device device): device_{device} {}

    Sampler const* SamplerCache::get(SamplerParams const& params) {
      auto lock = std::scoped_lock{mutex_};
      if (auto it = samplers_.find(params); it != samplers_.end()) {
        return &*it;
      } else {
        return &*samplers_.emplace(device_, params).first;
      }
    }
  } // namespace gpu
} // namespace mobula