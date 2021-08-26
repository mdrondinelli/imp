// clang-format off
module;
#include <Eigen/Dense>
#include <boost/container_hash/hash.hpp>
export module mobula.engine.util:Extent;
import <compare>;
// clang-format on

namespace mobula {
  /**
   * \brief Two-dimensional extent.
   */
  export template<typename T>
  struct Extent2 {
    T width;
    T height;

    constexpr Extent2() noexcept: width{}, height{} {}

    constexpr explicit Extent2(T width, T height) noexcept:
        width{width}, height{height} {}

    friend constexpr auto
    operator<=>(Extent2<T> const &lhs, Extent2<T> const &rhs) = default;
  };

  /**
   * \brief Three-dimensional extent.
   */
  export template<typename T>
  struct Extent3 {
    T width;
    T height;
    T depth;

    constexpr Extent3() noexcept: width{}, height{}, depth{} {}

    constexpr explicit Extent3(T width, T height, T depth) noexcept:
        width{width}, height{height}, depth{depth} {}

    friend constexpr auto
    operator<=>(Extent3<T> const &lhs, Extent3<T> const &rhs) = default;
  };

  export template<typename T>
  constexpr std::size_t hash_value(Extent2<T> extent) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, extent.width);
    boost::hash_combine(seed, extent.height);
    return seed;
  }

  export template<typename T>
  constexpr std::size_t hash_value(Extent3<T> extent) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, extent.width);
    boost::hash_combine(seed, extent.height);
    boost::hash_combine(seed, extent.depth);
    return seed;
  }

  export using Extent2f = Extent2<float>;
  export using Extent3f = Extent3<float>;
  export using Extent2i = Extent2<std::int32_t>;
  export using Extent3i = Extent3<std::int32_t>;
  export using Extent2u = Extent2<std::uint32_t>;
  export using Extent3u = Extent3<std::uint32_t>;
} // namespace mobula