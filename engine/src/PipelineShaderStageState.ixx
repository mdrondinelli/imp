// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:PipelineShaderStageState;
import <filesystem>;
import <string>;
import <variant>;
import <utility>;
import <vector>;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  export struct PipelineShaderStageState {
    std::filesystem::path module;
    std::string entryPoint;
    std::vector<std::pair<
        std::uint32_t,
        std::variant<bool, float, std::int32_t, std::uint32_t>>>
        specializationConstants;

    friend bool operator==(
        PipelineShaderStageState const &lhs,
        PipelineShaderStageState const &rhs) = default;
  };

  export std::size_t
  hash_value(PipelineShaderStageState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.module);
    hash_combine(seed, state.entryPoint);
    for (auto &[id, value] : state.specializationConstants) {
      hash_combine(seed, id);
      switch (value.index()) {
      case 0:
        hash_combine(seed, std::get<0>(value));
        break;
      case 1:
        hash_combine(seed, std::get<1>(value));
        break;
      case 2:
        hash_combine(seed, std::get<2>(value));
        break;
      case 3:
        hash_combine(seed, std::get<3>(value));
        break;
      }
    }
    return seed;
  }

  export void copyPipelineShaderStageState(
      vk::PipelineShaderStageCreateInfo &dstStage,
      vk::SpecializationInfo &dstSpecializationInfo,
      std::vector<vk::SpecializationMapEntry> &dstMapEntries,
      std::vector<std::byte> &dstData,
      ShaderModuleCache &shaderModuleCache,
      vk::ShaderStageFlagBits srcStage,
      PipelineShaderStageState const &srcState) {
    dstStage.stage = srcStage;
    dstStage.module = shaderModuleCache.create(srcState.module)->getHandle();
    dstStage.pName = srcState.entryPoint.c_str();
    if (!srcState.specializationConstants.empty()) {
      dstMapEntries.clear();
      dstMapEntries.reserve(srcState.specializationConstants.size());
      dstData.clear();
      dstData.reserve(4 * srcState.specializationConstants.size());
      auto offset = std::uint32_t{};
      for (auto &[id, value] : srcState.specializationConstants) {
        dstMapEntries.emplace_back(id, offset, 4);
        dstData.resize(offset + 4);
        switch (value.index()) {
        case 0: {
          auto b = vk::Bool32{std::get<0>(value) ? 1u : 0u};
          std::memcpy(&dstData[offset], &b, 4);
          break;
        }
        case 1:
          std::memcpy(&dstData[offset], &std::get<1>(value), 4);
          break;
        case 2:
          std::memcpy(&dstData[offset], &std::get<2>(value), 4);
          break;
        case 3:
          std::memcpy(&dstData[offset], &std::get<3>(value), 4);
          break;
        }
        offset += 4;
      }
      dstSpecializationInfo.mapEntryCount =
          static_cast<std::uint32_t>(dstMapEntries.size());
      dstSpecializationInfo.pMapEntries = dstMapEntries.data();
      dstSpecializationInfo.dataSize = dstData.size();
      dstSpecializationInfo.pData = dstData.data();
      dstStage.pSpecializationInfo = &dstSpecializationInfo;
    }
  }
} // namespace mobula