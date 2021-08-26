// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.engine.vulkan;
// clang-format on

namespace mobula {
  Sampler::Sampler(vk::Device device, SamplerParams const &params):
      params_{params} {
    auto createInfo = vk::SamplerCreateInfo{};
    createInfo.magFilter = params.magFilter;
    createInfo.minFilter = params.minFilter;
    createInfo.mipmapMode = params.mipmapFilter;
    createInfo.addressModeU = params.addressModeU;
    createInfo.addressModeV = params.addressModeV;
    createInfo.addressModeW = params.addressModeW;
    createInfo.mipLodBias = params.lodBias;
    if (params.anisotropy) {
      createInfo.anisotropyEnable = true;
      createInfo.maxAnisotropy = *params.anisotropy;
    }
    if (params.compareOp) {
      createInfo.compareEnable = true;
      createInfo.compareOp = *params.compareOp;
    }
    createInfo.minLod = params.minLod;
    createInfo.maxLod = params.maxLod;
    createInfo.borderColor = params.borderColor;
    handle_ = device.createSamplerUnique(createInfo);
  }
} // namespace mobula