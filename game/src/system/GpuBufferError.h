#pragma once

#include <exception>
#include <string_view>

namespace imp {

  class GpuBufferError: public std::exception {
  public:
    GpuBufferError(std::string_view what): what_{what} {}

    char const *what() const noexcept override {
      return what_.c_str();
    }

  private:
    std::string what_;
  };
} // namespace imp