#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

namespace imp {
  template<typename T>
  struct Extent2 {
    T width;
    T height;

    constexpr Extent2(T width, T height) noexcept:
        width{width}, height{height} {}

    constexpr Extent2(vk::Extent2D other) noexcept:
        width{static_cast<T>(other.width)},
        height{static_cast<T>(other.height)} {}

    constexpr operator vk::Extent2D() const noexcept {
      return vk::Extent2D{
          static_cast<std::uint32_t>(width),
          static_cast<std::uint32_t>(height)};
    }
  };

  template<typename T>
  struct Extent3 {
    T width;
    T height;
    T depth;

    constexpr Extent3(T width, T height, T depth) noexcept:
        width{width}, height{height}, depth{depth} {}

    constexpr Extent3(Extent2<T> const &other) noexcept:
        width{other.width}, height{other.height}, depth{1} {}

    constexpr Extent3(Extent2<T> const &other, T depth) noexcept:
        width{other.width}, height{other.height}, depth{depth} {}

    constexpr Extent3(vk::Extent3D other) noexcept:
        width{static_cast<T>(other.width)},
        height{static_cast<T>(other.height)},
        depth{static_cast<T>(other.depth)} {}

    constexpr operator vk::Extent3D() const noexcept {
      return vk::Extent3D{
          static_cast<std::uint32_t>(width),
          static_cast<std::uint32_t>(height),
          static_cast<std::uint32_t>(depth)};
    }
  };

  template<typename T>
  constexpr bool
  operator==(Extent2<T> const &lhs, Extent2<T> const &rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height;
  }

  template<typename T>
  constexpr bool
  operator!=(Extent2<T> const &lhs, Extent2<T> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  template<typename T>
  constexpr bool
  operator==(Extent3<T> const &lhs, Extent3<T> const &rhs) noexcept {
    return lhs.width == rhs.width && lhs.height == rhs.height &&
           lhs.depth == rhs.depth;
  }

  template<typename T>
  constexpr bool
  operator!=(Extent3<T> const &lhs, Extent3<T> const &rhs) noexcept {
    return !(lhs == rhs);
  }

  using Extent2i = Extent2<std::int32_t>;
  using Extent2u = Extent2<std::uint32_t>;
  using Extent2f = Extent2<float>;
  using Extent3i = Extent3<std::int32_t>;
  using Extent3u = Extent3<std::uint32_t>;
  using Extent3f = Extent3<float>;
} // namespace imp