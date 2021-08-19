// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.engine.gpu;
import <cstddef>;
import <cstring>;
// clang-format on

namespace mobula {
  ComputePipeline::ComputePipeline(
      vk::Device device,
      ShaderModuleCache &shaderModuleCache,
      ComputePipelineParams const &params):
      params_{params} {
    auto createInfo = vk::ComputePipelineCreateInfo{};
    createInfo.flags = params.flags;
    auto specializationInfo = vk::SpecializationInfo{};
    auto mapEntries = std::vector<vk::SpecializationMapEntry>{};
    auto data = std::vector<std::byte>{};
    copyPipelineShaderStageParams(
        createInfo.stage,
        specializationInfo,
        mapEntries,
        data,
        shaderModuleCache,
        vk::ShaderStageFlagBits::eCompute,
        params.computeStage);
    createInfo.layout = params.layout->getHandle();
    handle_ = device.createComputePipelineUnique({}, createInfo).value;
  }
} // namespace mobula