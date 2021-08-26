// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.engine.vulkan:Sampler;
import :SamplerParams;
// clang-format on

namespace mobula {
  /**
   * \brief Wrapper around Vulkan sampler.
   */
  export class Sampler {
  public:
    explicit Sampler(vk::Device device, SamplerParams const &params);

    SamplerParams const &getParams() const noexcept {
      return params_;
    }

    vk::Sampler getHandle() const noexcept {
      return *handle_;
    }

  private:
    SamplerParams params_;
    vk::UniqueSampler handle_;
  };
} // namespace mobula