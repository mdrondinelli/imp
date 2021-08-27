// clang-format off
module;
#include <boost/container_hash/hash.hpp>
#include <Eigen/Dense>
export module mobula.util:Matrix;
// clang-format on

namespace mobula {
  export template<typename T, int Rows, int Cols>
  using Matrix = Eigen::Matrix<T, Rows, Cols>;
  export template<typename T>
  using Matrix1 = Matrix<T, 1, 1>;
  export using Matrix1f = Matrix1<float>;
  export using Matrix1i = Matrix1<std::int32_t>;
  export using Matrix1u = Matrix1<std::uint32_t>;
  export template<typename T>
  using Matrix2 = Matrix<T, 2, 2>;
  export using Matrix2f = Matrix2<float>;
  export using Matrix2i = Matrix2<std::int32_t>;
  export using Matrix2u = Matrix2<std::uint32_t>;
  export template<typename T>
  using Matrix3 = Matrix<T, 3, 3>;
  export using Matrix3f = Matrix3<float>;
  export using Matrix3i = Matrix3<std::int32_t>;
  export using Matrix3u = Matrix3<std::uint32_t>;
  export template<typename T>
  using Matrix4 = Matrix<T, 4, 4>;
  export using Matrix4f = Matrix4<float>;
  export using Matrix4i = Matrix4<std::int32_t>;
  export using Matrix4u = Matrix4<std::uint32_t>;
  export template<typename T>
  using MatrixX = Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
  export using MatrixXf = MatrixX<float>;
  export using MatrixXi = MatrixX<std::int32_t>;
  export using MatrixXu = MatrixX<std::uint32_t>;

  export template<typename T, int Rows, int Cols>
  std::size_t hash_value(Matrix<T, Rows, Cols> const &x) noexcept {
    return boost::hash_range(x.data(), x.data() + x.size());
  }
} // namespace mobula