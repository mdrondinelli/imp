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
    createInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    createInfo.stage.module =
        shaderModuleCache.create(params.computeShaderState.modulePath)
            ->getHandle();
    createInfo.stage.pName = params.computeShaderState.entryPoint.c_str();
    auto specializationInfo = vk::SpecializationInfo{};
    auto mapEntries = std::vector<vk::SpecializationMapEntry>{};
    auto data = std::vector<std::byte>{};
    if (!params.computeShaderState.specializationConstants.empty()) {
      mapEntries.reserve(
          params.computeShaderState.specializationConstants.size());
      for (auto i = std::size_t{};
           i < params.computeShaderState.specializationConstants.size();
           ++i) {
        auto &[id, value] =
            params.computeShaderState.specializationConstants[i];
        auto &mapEntry = mapEntries.emplace_back();
        mapEntry.constantID = id;
        mapEntry.offset = static_cast<std::uint32_t>(4 * i);
        mapEntry.size = 4;
        switch (value.index()) {
        case 0:
          std::memcpy(
              data.data() + mapEntry.offset,
              &std::get<0>(value),
              mapEntry.size);
          break;
        case 1:
          std::memcpy(
              data.data() + mapEntry.offset,
              &std::get<1>(value),
              mapEntry.size);
          break;
        case 2:
          std::memcpy(
              data.data() + mapEntry.offset,
              &std::get<2>(value),
              mapEntry.size);
          break;
        case 3:
          std::memcpy(
              data.data() + mapEntry.offset,
              &std::get<3>(value),
              mapEntry.size);
          break;
        }
      }
      specializationInfo.mapEntryCount =
          static_cast<std::uint32_t>(mapEntries.size());
      specializationInfo.pMapEntries = mapEntries.data();
      specializationInfo.dataSize = data.size();
      specializationInfo.pData = data.data();
      createInfo.stage.pSpecializationInfo = &specializationInfo;
    }
    createInfo.layout = params.layout->getHandle();
    handle_ = device.createComputePipelineUnique({}, createInfo).value;
  }
} // namespace mobula