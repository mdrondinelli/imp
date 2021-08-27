// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.gpu:SamplerParams;
import <optional>;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Holds the parameters of a sampler.
     */
    export struct SamplerParams {
      vk::Filter magFilter;
      vk::Filter minFilter;
      vk::SamplerMipmapMode mipmapFilter;
      vk::SamplerAddressMode addressModeU;
      vk::SamplerAddressMode addressModeV;
      vk::SamplerAddressMode addressModeW;
      vk::BorderColor borderColor;
      float lodBias;
      float minLod;
      float maxLod;
      std::optional<float> anisotropy;
      std::optional<vk::CompareOp> compareOp;
    };

    export bool
    operator==(SamplerParams const &lhs, SamplerParams const &rhs) noexcept {
      return lhs.magFilter == rhs.magFilter && lhs.minFilter == rhs.minFilter &&
             lhs.mipmapFilter == rhs.mipmapFilter &&
             lhs.addressModeU == rhs.addressModeU &&
             lhs.addressModeV == rhs.addressModeV &&
             lhs.addressModeW == rhs.addressModeW &&
             (lhs.addressModeU != vk::SamplerAddressMode::eClampToBorder &&
                  lhs.addressModeV != vk::SamplerAddressMode::eClampToBorder &&
                  lhs.addressModeW != vk::SamplerAddressMode::eClampToBorder ||
              lhs.borderColor == rhs.borderColor) &&
             lhs.lodBias == rhs.lodBias && lhs.minLod == rhs.minLod &&
             lhs.maxLod == rhs.maxLod && lhs.anisotropy == rhs.anisotropy &&
             lhs.compareOp == rhs.compareOp;
    }

    export std::size_t hash_value(SamplerParams const &params) noexcept {
      using boost::hash_combine;
      auto seed = std::size_t{};
      hash_combine(seed, params.magFilter);
      hash_combine(seed, params.minFilter);
      hash_combine(seed, params.mipmapFilter);
      hash_combine(seed, params.addressModeU);
      hash_combine(seed, params.addressModeV);
      hash_combine(seed, params.addressModeW);
      if (params.addressModeU == vk::SamplerAddressMode::eClampToBorder ||
          params.addressModeV == vk::SamplerAddressMode::eClampToBorder ||
          params.addressModeW == vk::SamplerAddressMode::eClampToBorder) {
        hash_combine(seed, params.borderColor);
      }
      hash_combine(seed, params.lodBias);
      hash_combine(seed, params.minLod);
      hash_combine(seed, params.maxLod);
      hash_combine(seed, params.anisotropy);
      hash_combine(seed, params.compareOp);
      return seed;
    }
  } // namespace gpu
} // namespace mobula