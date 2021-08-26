// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.engine.vulkan;
import <mutex>;
// clang-format on

namespace mobula {
  DescriptorSetLayoutCache::DescriptorSetLayoutCache(vk::Device device):
      device_{device} {}

  DescriptorSetLayout const *
  DescriptorSetLayoutCache::get(DescriptorSetLayoutParams const &params) {
    auto lock = std::scoped_lock{mutex_};
    if (auto it = cache_.find(params); it != cache_.end()) {
      return &*it;
    } else {
      return &*cache_.emplace(device_, params).first;
    }
  }
} // namespace mobula