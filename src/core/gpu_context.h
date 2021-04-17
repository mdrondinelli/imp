#pragma once

#include <boost/container_hash/hash.hpp>

#include "gpu_buffer.h"
#include "gpu_image.h"

namespace std {
  template<>
  struct hash<vk::SamplerCreateInfo> {
    size_t operator()(vk::SamplerCreateInfo const &create_info) const {
      if (create_info.pNext) {
        throw std::runtime_error{
            "tried to hash vk::SamplerCreateInfo with non-null pNext."};
      }
      if (create_info.flags) {
        throw std::runtime_error{
            "tried to hash vk::SamplerCreateInfo with non-zero flags."};
      }
      auto seed = size_t{};
      boost::hash_combine(seed, create_info.magFilter);
      boost::hash_combine(seed, create_info.minFilter);
      boost::hash_combine(seed, create_info.mipmapMode);
      boost::hash_combine(seed, create_info.addressModeU);
      boost::hash_combine(seed, create_info.addressModeV);
      boost::hash_combine(seed, create_info.addressModeW);
      boost::hash_combine(seed, create_info.mipLodBias);
      boost::hash_combine(seed, create_info.anisotropyEnable);
      boost::hash_combine(seed, create_info.maxAnisotropy);
      boost::hash_combine(seed, create_info.compareEnable);
      boost::hash_combine(seed, create_info.compareOp);
      boost::hash_combine(seed, create_info.minLod);
      boost::hash_combine(seed, create_info.maxLod);
      boost::hash_combine(seed, create_info.borderColor);
      boost::hash_combine(seed, create_info.unnormalizedCoordinates);
      return seed;
    }
  };
} // namespace std

namespace imp {
  class gpu_context {
  public:
    gpu_context(bool validation_enabled, bool presentation_enabled);
    ~gpu_context();

    bool validation_enabled() const noexcept;
    bool presentation_enabled() const noexcept;
    vk::Instance instance() const noexcept;
    vk::PhysicalDevice physical_device() const noexcept;
    uint32_t graphics_family() const noexcept;
    //uint32_t compute_family() const noexcept;
    uint32_t present_family() const noexcept;
    vk::Device device() const noexcept;
    vk::Queue graphics_queue() const noexcept;
    //vk::Queue compute_queue() const noexcept;
    vk::Queue present_queue() const noexcept;

    vk::Sampler create_sampler(vk::SamplerCreateInfo const &create_info);
    gpu_buffer create_buffer(
        vk::BufferCreateInfo const &buffer_info,
        VmaAllocationCreateInfo const &allocation_info);
    gpu_image create_image(
        vk::ImageCreateInfo const &image_info,
        VmaAllocationCreateInfo const &allocation_info);

  private:
    bool validation_enabled_;
    bool presentation_enabled_;
    vk::UniqueInstance instance_;
    vk::PhysicalDevice physical_device_;
    uint32_t graphics_family_;
    //uint32_t compute_family_;
    uint32_t present_family_;
    vk::UniqueDevice device_;
    vk::Queue graphics_queue_;
    //vk::Queue compute_queue_;
    vk::Queue present_queue_;
    std::unordered_map<vk::SamplerCreateInfo, vk::UniqueSampler> samplers_;
    VmaAllocator allocator_;

    vk::UniqueInstance create_instance();
    vk::PhysicalDevice select_physical_device();
    uint32_t select_graphics_family();
    //uint32_t select_compute_family();
    uint32_t select_present_family();
    vk::UniqueDevice create_device();
    vk::Queue select_graphics_queue();
    //vk::Queue select_compute_queue();
    vk::Queue select_present_queue();
    VmaAllocator create_allocator();
  };
} // namespace imp