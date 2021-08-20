// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <Eigen/Dense>
export module mobula.engine.util:AlignedBox;
import <limits>;
import :Matrix;
import :Vector;
// clang-format on

namespace mobula {
  /**
   * \brief Axis-aligned bounding box.
   */
  export template<typename Scalar, int Dimension>
  class AlignedBox {
  public:
    using VectorType = Vector<Scalar, Dimension>;

    AlignedBox() noexcept:
        min_{VectorType::Constant(std::numeric_limits<Scalar>::max())},
        max_{VectorType::Constant(std::numeric_limits<Scalar>::lowest())} {}

    AlignedBox(VectorType const &p) noexcept: min_{p}, max_{p} {}

    AlignedBox(VectorType const &min, VectorType const &max) noexcept:
        min_{min}, max_{max} {}

    VectorType const &min() const noexcept {
      return min_;
    }

    VectorType &min() noexcept {
      return min_;
    }

    VectorType const &max() const noexcept {
      return max_;
    }

    VectorType &max() noexcept {
      return max_;
    }

    auto diagonal() const noexcept {
      return max_ - min_;
    }

    bool operator==(AlignedBox const &rhs) const = default;

  private:
    VectorType min_;
    VectorType max_;
  };

  export template<typename Scalar, int Dimension>
  std::size_t
  hash_value(AlignedBox<Scalar, Dimension> const &alignedBox) noexcept {
    using boost::hash_combine;
    auto seed = std::size_t{};
    hash_combine(seed, hash_value(alignedBox.min()));
    hash_combine(seed, hash_value(alignedBox.max()));
    return seed;
  }

  export using AlignedBox1f = AlignedBox<float, 1>;
  export using AlignedBox2f = AlignedBox<float, 2>;
  export using AlignedBox3f = AlignedBox<float, 3>;
  export using AlignedBox4f = AlignedBox<float, 4>;
  export using AlignedBoxXf = AlignedBox<float, Eigen::Dynamic>;
  export using AlignedBox1i = AlignedBox<std::int32_t, 1>;
  export using AlignedBox2i = AlignedBox<std::int32_t, 2>;
  export using AlignedBox3i = AlignedBox<std::int32_t, 3>;
  export using AlignedBox4i = AlignedBox<std::int32_t, 4>;
  export using AlignedBoxXi = AlignedBox<std::int32_t, Eigen::Dynamic>;
  export using AlignedBox1u = AlignedBox<std::uint32_t, 1>;
  export using AlignedBox2u = AlignedBox<std::uint32_t, 2>;
  export using AlignedBox3u = AlignedBox<std::uint32_t, 3>;
  export using AlignedBox4u = AlignedBox<std::uint32_t, 4>;
  export using AlignedBoxXu = AlignedBox<std::uint32_t, Eigen::Dynamic>;
} // namespace mobula