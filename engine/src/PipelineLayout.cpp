// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.gpu;
import <vector>;
// clang-format on

namespace mobula {
  namespace gpu {
    PipelineLayout::PipelineLayout(
        vk::Device device, PipelineLayoutParams const &params):
        params_{params} {
      auto vkCreateInfo = vk::PipelineLayoutCreateInfo{};
      auto vkSetLayouts = std::vector<vk::DescriptorSetLayout>{};
      if (!params.setLayouts.empty()) {
        vkSetLayouts.reserve(params.setLayouts.size());
        for (auto setLayout : params.setLayouts) {
          vkSetLayouts.emplace_back(setLayout->getHandle());
        }
        vkCreateInfo.setLayoutCount =
            static_cast<std::uint32_t>(vkSetLayouts.size());
        vkCreateInfo.pSetLayouts = vkSetLayouts.data();
      }
      auto vkPushConstantRanges = std::vector<vk::PushConstantRange>{};
      if (!params.pushConstantRanges.empty()) {
        vkPushConstantRanges.reserve(params.pushConstantRanges.size());
        for (auto &pushConstantRange : params.pushConstantRanges) {
          vkPushConstantRanges.emplace_back(
              pushConstantRange.stageFlags,
              pushConstantRange.offset,
              pushConstantRange.size);
        }
        vkCreateInfo.pushConstantRangeCount =
            static_cast<std::uint32_t>(vkPushConstantRanges.size());
        vkCreateInfo.pPushConstantRanges = vkPushConstantRanges.data();
      }
      handle_ = device.createPipelineLayoutUnique(vkCreateInfo);
    }
  } // namespace gpu
} // namespace mobula