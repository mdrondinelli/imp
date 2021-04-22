#include "static_mesh_loader.h"

#include <fstream>

namespace imp {
  std::optional<static_mesh>
  static_mesh_loader::load(std::filesystem::path const &path) {
    try {
      auto in = std::ifstream{};
      in.exceptions(std::ios::badbit | std::ios::failbit);
      in.open(path, std::ios::binary);
      auto vertex_count = std::uint32_t{};
      auto index_count = std::uint32_t{};
      in.read(reinterpret_cast<char *>(&vertex_count), 4);
      in.read(reinterpret_cast<char *>(&index_count), 4);

    } catch (...) {
      return std::nullopt;
    }
  }
} // namespace imp