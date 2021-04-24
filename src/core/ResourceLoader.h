#pragma once

#include <optional>

namespace imp {
  template<typename Resource, typename ResourceInfo>
  class ResourceLoader {
  public:
    virtual ~ResourceLoader() = default;
    virtual std::optional<Resource> load(ResourceInfo const &info) = 0;
  };
} // namespace imp