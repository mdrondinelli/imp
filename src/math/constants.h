#pragma once

namespace imp {
  template<typename T>
  class constants {
  public:
    static constexpr T const e = static_cast<T>(2.71828182845904523536);
    static constexpr T const pi = static_cast<T>(3.14159265358979323846);
  };
} // namespace imp