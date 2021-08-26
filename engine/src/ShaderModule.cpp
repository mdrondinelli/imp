// clang-format off
module;
#include <vulkan/vulkan.hpp>
module mobula.engine.vulkan;
import <filesystem>;
import <fstream>;
import <vector>;
// clang-format on

namespace mobula {
  ShaderModule::ShaderModule(
      vk::Device device, std::filesystem::path const &path):
      path_{path} {
    auto code = std::vector<char>{};
    {
      auto ifs = std::ifstream{};
      ifs.exceptions(std::ios::badbit | std::ios::failbit);
      ifs.open(path, std::ios::binary);
      ifs.seekg(0, std::ios::end);
      code.resize(static_cast<std::size_t>(ifs.tellg()));
      ifs.seekg(0, std::ios::beg);
      ifs.read(code.data(), code.size());
    }
    auto createInfo = vk::ShaderModuleCreateInfo{};
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<std::uint32_t *>(code.data());
    handle_ = device.createShaderModuleUnique(createInfo);
  }
} // namespace mobula