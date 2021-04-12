#pragma once

#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>

#include "../math/scalar.h"

namespace imp {
  struct gpu_sampler_info {
    vk::Filter mag_filter;
    vk::Filter min_filter;
    vk::SamplerMipmapMode mipmap_mode;
    vk::SamplerAddressMode address_mode_u;
    vk::SamplerAddressMode address_mode_v;
    vk::SamplerAddressMode address_mode_w;
    float mip_lod_bias;
    bool anisotropy_enable;
    bool compare_enable;
    vk::CompareOp compare_op;
    float min_lod;
    float max_lod;
    vk::BorderColor border_color;
    bool unnormalized_coordinates;
  };

  constexpr bool operator==(
      gpu_sampler_info const &lhs, gpu_sampler_info const &rhs) noexcept {
    return lhs.mag_filter == rhs.mag_filter &&
           lhs.min_filter == rhs.min_filter &&
           lhs.mipmap_mode == rhs.mipmap_mode &&
           lhs.address_mode_u == rhs.address_mode_u &&
           lhs.address_mode_v == rhs.address_mode_v &&
           lhs.address_mode_w == rhs.address_mode_w &&
           lhs.mip_lod_bias == rhs.mip_lod_bias &&
           lhs.anisotropy_enable == rhs.anisotropy_enable &&
           lhs.compare_enable == rhs.compare_enable &&
           lhs.compare_op == rhs.compare_op && lhs.min_lod == rhs.min_lod &&
           lhs.max_lod == rhs.max_lod && lhs.border_color == rhs.border_color &&
           lhs.unnormalized_coordinates == rhs.unnormalized_coordinates;
  }

  constexpr bool operator!=(
      gpu_sampler_info const &lhs, gpu_sampler_info const &rhs) noexcept {
    return !(lhs == rhs);
  }

  inline size_t hash_value(gpu_sampler_info const &info) noexcept {
    auto seed = size_t{};
    boost::hash_combine(seed, info.mag_filter);
    boost::hash_combine(seed, info.min_filter);
    boost::hash_combine(seed, info.mipmap_mode);
    boost::hash_combine(seed, info.address_mode_u);
    boost::hash_combine(seed, info.address_mode_v);
    boost::hash_combine(seed, info.address_mode_w);
    boost::hash_combine(seed, info.mip_lod_bias);
    boost::hash_combine(seed, info.anisotropy_enable);
    boost::hash_combine(seed, info.compare_enable);
    boost::hash_combine(seed, info.compare_op);
    boost::hash_combine(seed, info.min_lod);
    boost::hash_combine(seed, info.max_lod);
    boost::hash_combine(seed, info.border_color);
    boost::hash_combine(seed, info.unnormalized_coordinates);
    return seed;
  }
} // namespace imp

namespace std {
  template<>
  struct hash<imp::gpu_sampler_info> {
    size_t operator()(imp::gpu_sampler_info const &info) const noexcept {
      return hash_value(info);
    }
  };
} // namespace std