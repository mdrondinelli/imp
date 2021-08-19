// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.engine.gpu;
// clang-format on

namespace mobula {
  DescriptorSetLayout::DescriptorSetLayout(
      vk::Device device, DescriptorSetLayoutParams const &params):
      params_{params} {
    auto vkCreateInfo = vk::DescriptorSetLayoutCreateInfo{};
    vkCreateInfo.flags = params.flags;
    auto vkBindings = std::vector<vk::DescriptorSetLayoutBinding>{};
    if (!params.bindings.empty()) {
      vkBindings.reserve(params.bindings.size());
      for (auto &binding : params.bindings) {
        auto vkBinding = vkBindings.emplace_back();
        vkBinding.binding = static_cast<std::uint32_t>(vkBindings.size() - 1);
        vkBinding.descriptorType = binding.descriptorType;
        vkBinding.descriptorCount = binding.descriptorCount;
        vkBinding.stageFlags = binding.stageFlags;
      }
      vkCreateInfo.bindingCount = static_cast<std::uint32_t>(vkBindings.size());
      vkCreateInfo.pBindings = vkBindings.data();
    }
    handle_ = device.createDescriptorSetLayoutUnique(vkCreateInfo);
  }
} // namespace mobula