// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.gpu;
import <mutex>;
// clang-format on

namespace mobula {
  namespace gpu {
    PipelineLayoutCache::PipelineLayoutCache(vk::Device device):
        device_{device} {}

    PipelineLayout const *
    PipelineLayoutCache::get(PipelineLayoutParams const &params) {
      auto lock = std::scoped_lock{mutex_};
      if (auto it = pipelineLayouts_.find(params);
          it != pipelineLayouts_.end()) {
        return &*it;
      } else {
        return &*pipelineLayouts_.emplace(device_, params).first;
      }
    }
  } // namespace gpu
} // namespace mobula