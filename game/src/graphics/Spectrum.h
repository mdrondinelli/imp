#pragma once

#include <Eigen/Dense>

namespace imp {
  class Spectrum {
  public:
    static Spectrum const BLACK;
    static Spectrum const WHITE;

    Spectrum() = default;

    Spectrum(float v) noexcept: rgb_{v, v, v} {}

    Spectrum(float r, float g, float b) noexcept: rgb_{r, g, b} {}

    float const *data() const noexcept {
      return rgb_.data();
    }

    float *data() noexcept {
      return rgb_.data();
    }

    float const &operator()(std::size_t i) const noexcept {
      return rgb_(i);
    }

    float &operator()(std::size_t i) noexcept {
      return rgb_(i);
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