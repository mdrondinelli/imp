#pragma once

#include <unordered_map>

#include "gpu_sampler_create_info.h"

namespace imp {
  class gpu_sampler_pool {
  public:
    gpu_sampler_pool(vk::Device device) noexcept;
    vk::Sampler create(gpu_sampler_create_info const &info);

  private:
    vk::Device device_;
    std::unordered_map<gpu_sampler_create_info, vk::UniqueSampler> samplers_;
  };
} // namespace imp