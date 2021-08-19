// clang-format off
module;
#include <boost/container_hash/hash.hpp>
export module mobula.engine.gpu:PipelineShaderState;
import <filesystem>;
import <string>;
import <variant>;
import <utility>;
import <vector>;
// clang-format on

namespace mobula {
  export struct PipelineShaderState {
    std::filesystem::path modulePath;
    std::string entryPoint;
    std::vector<std::pair<
        std::uint32_t,
        std::variant<bool, float, std::int32_t, std::uint32_t>>>
        specializationConstants;

    friend bool operator==(
        PipelineShaderState const &lhs,
        PipelineShaderState const &rhs) = default;
  };

  export std::size_t hash_value(PipelineShaderState const &state) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, state.modulePath);
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
} // namespace mobula