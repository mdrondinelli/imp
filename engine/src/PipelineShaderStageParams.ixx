// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <vulkan/vulkan.hpp>
export module mobula.engine.gpu:PipelineShaderStageParams;
import <filesystem>;
import <string>;
import <variant>;
import <utility>;
import <vector>;
import :ShaderModuleCache;
// clang-format on

namespace mobula {
  /**
   * \brief Holds the parameters of a shader stage of a pipeline.
   */
  export struct PipelineShaderStageParams {
    std::filesystem::path module;
    std::string entryPoint;
    std::vector<std::pair<
        std::uint32_t,
        std::variant<bool, float, std::int32_t, std::uint32_t>>>
        specializationConstants;

    bool operator==(PipelineShaderStageParams const &rhs) const = default;
  };

  export std::size_t
  hash_value(PipelineShaderStageParams const &params) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, params.module);
    hash_combine(seed, params.entryPoint);
    for (auto &[id, value] : params.specializationConstants) {
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

  export void copyPipelineShaderStageParams(
      vk::PipelineShaderStageCreateInfo &dstStage,
      vk::SpecializationInfo &dstSpecializationInfo,
      std::vector<vk::SpecializationMapEntry> &dstMapEntries,
      std::vector<std::byte> &dstData,
      ShaderModuleCache &shaderModuleCache,
      vk::ShaderStageFlagBits srcStage,
      PipelineShaderStageParams const &srcParams) {
    dstStage.stage = srcStage;
    dstStage.module = shaderModuleCache.get(srcParams.module)->getHandle();
    dstStage.pName = srcParams.entryPoint.c_str();
    if (!srcParams.specializationConstants.empty()) {
      dstMapEntries.clear();
      dstMapEntries.reserve(srcParams.specializationConstants.size());
      dstData.clear();
      dstData.reserve(4 * srcParams.specializationConstants.size());
      auto offset = std::uint32_t{};
      for (auto &[id, value] : srcParams.specializationConstants) {
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