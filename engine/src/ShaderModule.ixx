// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:ShaderModule;
import <filesystem>;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around a vulkan shader module.
   */
  export class ShaderModule {
  public:
    explicit ShaderModule(vk::Device device, std::filesystem::path const &path);

    vk::ShaderModule getHandle() const noexcept {
      return *handle_;
    }

    std::filesystem::path const &getPath() const noexcept {
      return path_;
    }

  private:
    vk::UniqueShaderModule handle_;
    std::filesystem::path path_;
  };
} // namespace mobula