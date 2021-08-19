// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:ShaderModuleCache;
import <filesystem>;
import <mutex>;
import <unordered_set>;
export import :ShaderModule;
// clang-format on

namespace mobula {
  export class ShaderModuleCache {
  public:
    explicit ShaderModuleCache(vk::Device device);

    void clear() noexcept;
    
    ShaderModule const *create(std::filesystem::path const &path);

  private:
    struct Hash {
      using is_transparent = void;

      std::size_t operator()(ShaderModule const &module) const noexcept {
        return boost::hash<std::filesystem::path>{}(module.getPath());
      }

      std::size_t operator()(std::filesystem::path const &path) const noexcept {
        return boost::hash<std::filesystem::path>{}(path);
      }
    };

    struct Equal {
      using is_transparent = void;

      bool operator()(
          ShaderModule const &lhs, ShaderModule const &rhs) const noexcept {
        return &lhs == &rhs;
      }

      bool operator()(ShaderModule const &lhs, std::filesystem::path const &rhs)
          const noexcept {
        return lhs.getPath() == rhs;
      }

      bool operator()(std::filesystem::path const &lhs, ShaderModule const &rhs)
          const noexcept {
        return lhs == rhs.getPath();
      }
    };

    vk::Device device_;
    std::unordered_set<ShaderModule, Hash, Equal> modules_;
    std::mutex mutex_;
  };
} // namespace mobula