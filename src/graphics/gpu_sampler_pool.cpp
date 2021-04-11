#include "gpu_sampler_pool.h"

namespace imp {
  gpu_sampler_pool::gpu_sampler_pool(vk::Device device) noexcept:
      device_{device} {}

  vk::Sampler gpu_sampler_pool::create(gpu_sampler_create_info const &info) {
    if (auto it = samplers_.find(info); it != samplers_.end()) {
      return *it->second;
    }
    auto create_info = vk::SamplerCreateInfo{};
    create_info.magFilter = info.mag_filter;
    create_info.minFilter = info.min_filter;
    create_info.mipmapMode = info.mipmap_mode;
    create_info.addressModeU = info.address_mode_u;
    create_info.addressModeV = info.address_mode_v;
    create_info.addressModeW = info.address_mode_w;
    create_info.mipLodBias = info.mip_lod_bias;
    create_info.anisotropyEnable = info.anisotropy_enable;
    create_info.compareEnable = info.compare_enable;
    create_info.compareOp = info.compare_op;
    create_info.minLod = info.min_lod;
    create_info.maxLod = info.max_lod;
    create_info.borderColor = info.border_color;
    create_info.unnormalizedCoordinates = info.unnormalized_coordinates;
    return *samplers_.emplace(info, device_.createSamplerUnique(create_info))
                .first->second;
  }
} // namespace imp