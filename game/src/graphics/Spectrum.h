#pragma once

#include <Eigen/Dense>

namespace imp {
  class Spectrum {
  public:
    static Spectrum const BLACK;
    static Spectrum const WHITE;
    static Spectrum const RED;
    static Spectrum const GREEN;
    static Spectrum const BLUE;

    Spectrum() = default;

    Spectrum(float v) noexcept: rgb_{v, v, v} {}

    Spectrum(float r, float g, float b) noexcept: rgb_{r, g, b} {}

    float const &r() const noexcept {
      return rgb_(0);
    }

    float &r() noexcept {
      return rgb_(0);
    }

    float const &g() const noexcept {
      return rgb_(1);
    }

    float &g() noexcept {
      return rgb_(1);
    }

    float const &b() const noexcept {
      return rgb_(2);
    }

    float &b() noexcept {
      return rgb_(2);
    }

    float const &operator()(std::size_t i) const noexcept {
      return rgb_(i);
    }

    float &operator()(std::size_t i) noexcept {
      return rgb_(i);
    }

    float const *begin() const noexcept {
      return rgb_.data();
    }

    float *begin() noexcept {
      return rgb_.data();
    }

    float const *cbegin() const noexcept {
      return begin();
    }

    float const* end() const noexcept {
      return begin() + 3;
    }

    float* end() noexcept {
      return begin() + 3;
    }

    float const* cend() const noexcept {
      return begin() + 3;
    }

    float const *data() const noexcept {
      return rgb_.data();
    }

    float *data() noexcept {
      return rgb_.data();
    }

    friend Spectrum operator+(Spectrum const &s) noexcept {
      return s;
    }

    friend Spectrum operator-(Spectrum const &s) noexcept {
      Spectrum ret;
      ret.rgb_ = -s.rgb_;
      return ret;
    }

    friend Spectrum
    operator*(Spectrum const &lhs, Spectrum const &rhs) noexcept {
      Spectrum ret;
      ret.rgb_ = lhs.rgb_ + rhs.rgb_;
      return ret;
    }

    friend Spectrum &operator*=(Spectrum &lhs, Spectrum const &rhs) noexcept {
      return lhs = lhs * rhs;
    }

    friend Spectrum
    operator/(Spectrum const &lhs, Spectrum const &rhs) noexcept {
      Spectrum ret;
      ret.rgb_ = lhs.rgb_ / rhs.rgb_;
      return ret;
    }

    friend Spectrum &operator/=(Spectrum &lhs, Spectrum const &rhs) noexcept {
      return lhs = lhs / rhs;
    }

    friend Spectrum
    operator+(Spectrum const &lhs, Spectrum const &rhs) noexcept {
      Spectrum ret;
      ret.rgb_ = lhs.rgb_ + rhs.rgb_;
      return ret;
    }

    friend Spectrum &operator+=(Spectrum &lhs, Spectrum const &rhs) noexcept {
      return lhs = lhs + rhs;
    }

    friend Spectrum
    operator-(Spectrum const &lhs, Spectrum const &rhs) noexcept {
      Spectrum ret;
      ret.rgb_ = lhs.rgb_ - rhs.rgb_;
      return ret;
    }

    friend Spectrum &operator-=(Spectrum &lhs, Spectrum const &rhs) noexcept {
      return lhs = lhs - rhs;
    }

    friend bool operator==(Spectrum const &lhs, Spectrum const &rhs) noexcept {
      return (lhs.rgb_ == rhs.rgb_).all();
    }

    friend bool operator!=(Spectrum const &lhs, Spectrum const &rhs) noexcept {
      return !(lhs == rhs);
    }

  private:
    Eigen::Array3f rgb_;
  };
} // namespace imp