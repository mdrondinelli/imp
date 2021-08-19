// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.engine.gpu;
import <mutex>;
// clang-format on

namespace mobula {
  ComputePipelineCache::ComputePipelineCache(vk::Device device):
      device_{device}, shaderModuleCache_{device} {}

  void ComputePipelineCache::clearShaderModuleCache() noexcept {
    auto lock = std::scoped_lock{mutex_};
    shaderModuleCache_.clear();
  }

  ComputePipeline const *
  ComputePipelineCache::create(ComputePipelineParams const &params) {
    auto lock = std::scoped_lock{mutex_};
    if (auto it = computePipelines_.find(params);
        it != computePipelines_.end()) {
      return &*it;
    } else {
      return &*computePipelines_.emplace(device_, shaderModuleCache_, params)
                   .first;
    }
  }
} // namespace mobula