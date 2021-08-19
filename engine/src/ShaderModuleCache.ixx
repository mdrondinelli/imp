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
  /**
   * A class that creates and caches shader modules.
   */
  export class ShaderModuleCache {
  public:
    explicit ShaderModuleCache(vk::Device device);

    /**
     * If this function is called with params equal to the params of a previous
     * invocation, it returns the same shader module as the first invocation.
     * Otherwise, this function creates and returns a new shader module.
     *
     * \param path the path to a shader module.
     *
     * \return a pointer to the shader module at path \a path.
     */
    ShaderModule const *get(std::filesystem::path const &path);

    /**
     * This function clears the cache, freeing up memory but requiring
     * previously loaded shaders to be reloaded when requested.
     */
    void clear() noexcept;

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