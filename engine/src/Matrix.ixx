// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <Eigen/Dense>
export module mobula.engine.util:Matrix;
// clang-format on

namespace mobula {
  export template<typename Scalar, int Rows, int Cols>
  using Matrix = Eigen::Matrix<Scalar, Rows, Cols>;
  export using Matrix1f = Matrix<float, 1, 1>;
  export using Matrix2f = Matrix<float, 2, 2>;
  export using Matrix3f = Matrix<float, 3, 3>;
  export using Matrix4f = Matrix<float, 4, 4>;
  export using MatrixXf = Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
  export using Matrix1i = Matrix<std::int32_t, 1, 1>;
  export using Matrix2i = Matrix<std::int32_t, 2, 2>;
  export using Matrix3i = Matrix<std::int32_t, 3, 3>;
  export using Matrix4i = Matrix<std::int32_t, 4, 4>;
  export using MatrixXi = Matrix<std::int32_t, Eigen::Dynamic, Eigen::Dynamic>;
  export using Matrix1u = Matrix<std::uint32_t, 1, 1>;
  export using Matrix2u = Matrix<std::uint32_t, 2, 2>;
  export using Matrix3u = Matrix<std::uint32_t, 3, 3>;
  export using Matrix4u = Matrix<std::uint32_t, 4, 4>;
  export using MatrixXu = Matrix<std::uint32_t, Eigen::Dynamic, Eigen::Dynamic>;

  export template<typename Scalar, int Rows, int Cols>
  std::size_t hash_value(Matrix<Scalar, Rows, Cols> const &x) noexcept {
    return boost::hash_range(x.data(), x.data() + x.size());
  }
} // namespace mobula