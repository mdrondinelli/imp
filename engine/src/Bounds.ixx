// clang-format off
module;
#include <boost/container_hash/hash.hpp>
export module mobula.engine.util:Bounds;
import <limits>;
import :Vector;
// clang-format on

namespace mobula {
  export template<typename T>
  struct Bounds1 {
    T min;
    T max;

    constexpr Bounds1() noexcept:
        min{std::numeric_limits<T>::max()},
        max{std::numeric_limits<T>::lowest()} {}

    constexpr explicit Bounds1(T x) noexcept: min{x}, max{x} {}

    constexpr explicit Bounds1(T min, T max) noexcept: min{min}, max{max} {}

    constexpr operator bool() const noexcept {
      return min <= max;
    }

    friend constexpr bool
    operator==(Bounds1<T> const &lhs, Bounds1<T> const &rhs) = default;
  };

  export template<typename T>
  std::size_t hash_value(Bounds1<T> const &i) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, i.min);
    boost::hash_combine(seed, i.max);
    return seed;
  }

  export using Bounds1f = Bounds1<float>;
  export using Bounds1i = Bounds1<std::int32_t>;
  export using Bounds1u = Bounds1<std::uint32_t>;

  export template<typename T>
  struct Bounds2 {
    Vector2<T> min;
    Vector2<T> max;

    Bounds2() noexcept:
        min{Vector2<T>::Constant(std::numeric_limits<T>::max())},
        max{Vector2<T>::Constant(std::numeric_limits<T>::lowest())} {}

    explicit Bounds2(Vector2<T> const &x) noexcept: min{x}, max{x} {}

    explicit Bounds2(Vector2<T> const &min, Vector2<T> const &max) noexcept:
        min{min}, max{max} {}

    operator bool() const noexcept {
      return min.x() <= max.x() && min.y() <= max.y();
    }

    friend bool
    operator==(Bounds2<T> const &lhs, Bounds2<T> const &rhs) = default;
  };

  export template<typename T>
  std::size_t hash_value(Bounds2<T> const &i) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, hash_value(i.min));
    boost::hash_combine(seed, hash_value(i.max));
    return seed;
  }

  export using Bounds2f = Bounds2<float>;
  export using Bounds2i = Bounds2<std::int32_t>;
  export using Bounds2u = Bounds2<std::uint32_t>;

  export template<typename T>
  struct Bounds3 {
    Vector3<T> min;
    Vector3<T> max;

    Bounds3() noexcept:
        min{Vector3<T>::Constant(std::numeric_limits<T>::max())},
        max{Vector3<T>::Constant(std::numeric_limits<T>::lowest())} {}

    explicit Bounds3(Vector3<T> const &x) noexcept: min{x}, max{x} {}

    explicit Bounds3(Vector3<T> const &min, Vector3<T> const &max) noexcept:
        min{min}, max{max} {}

    operator bool() const noexcept {
      return min.x() <= max.x() && min.y() <= max.y() && min.z() <= max.z();
    }

    friend bool
    operator==(Bounds3<T> const &lhs, Bounds3<T> const &rhs) = default;
  };

  export template<typename T>
  std::size_t hash_value(Bounds3<T> const &i) noexcept {
    auto seed = std::size_t{};
    boost::hash_combine(seed, hash_value(i.min));
    boost::hash_combine(seed, hash_value(i.max));
    return seed;
  }

  export using Bounds3f = Bounds3<float>;
  export using Bounds3i = Bounds3<std::int32_t>;
  export using Bounds3u = Bounds3<std::uint32_t>;
} // namespace mobula