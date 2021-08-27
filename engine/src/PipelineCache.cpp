// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.gpu;
import <mutex>;
// clang-format on

namespace mobula {
  namespace gpu {
    PipelineCache::PipelineCache(vk::Device device):
        device_{device}, shaderModules_{device} {}

    ComputePipeline const *
    PipelineCache::get(ComputePipelineParams const &params) {
      auto lock = std::scoped_lock{computePipelinesMutex_};
      if (auto it = computePipelines_.find(params);
          it != computePipelines_.end()) {
        return &*it;
      } else {
        return &*computePipelines_.emplace(device_, shaderModules_, params)
                     .first;
      }
    }

    GraphicsPipeline const *
    PipelineCache::get(GraphicsPipelineParams const &params) {
      auto lock = std::scoped_lock{graphicsPipelinesMutex_};
      if (auto it = graphicsPipelines_.find(params);
          it != graphicsPipelines_.end()) {
        return &*it;
      } else {
        return &*graphicsPipelines_.emplace(device_, shaderModules_, params)
                     .first;
      }
    }

    void PipelineCache::clearShaderModules() noexcept {
      shaderModules_.clear();
    }
  } // namespace gpu
} // namespace mobula