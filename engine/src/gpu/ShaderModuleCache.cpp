// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.gpu;
import <mutex>;
// clang-format on

namespace mobula {
  namespace gpu {
    ShaderModuleCache::ShaderModuleCache(vk::Device device): device_{device} {}

    ShaderModule const *
    ShaderModuleCache::get(std::filesystem::path const &path) {
      auto lock = std::scoped_lock{mutex_};
      if (auto it = modules_.find(path); it != modules_.end()) {
        return &*it;
      } else {
        return &*modules_.emplace(device_, path).first;
      }
    }

    void ShaderModuleCache::clear() noexcept {
      auto lock = std::scoped_lock{mutex_};
      modules_.clear();
    }
  } // namespace gpu
} // namespace mobula