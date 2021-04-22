#pragma once

#include <optional>

namespace imp {
  template<typename Resource, typename ResourceInfo>
  class resource_loader {
  public:
    virtual ~resource_loader() = default;
    virtual std::optional<Resource> load(ResourceInfo const &info) = 0;
  };
} // namespace imp