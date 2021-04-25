#pragma once

namespace imp {
  template<typename T>
  class Constants {
  public:
    static constexpr T const E = static_cast<T>(2.71828182845904523536);
    static constexpr T const PI = static_cast<T>(3.14159265358979323846);
  };
} // namespace imp