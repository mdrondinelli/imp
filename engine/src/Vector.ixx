// clang-format off
module;
#include <Eigen/Dense>
export module mobula.util:Vector;
import :Matrix;
// clang-format on

namespace mobula {
  export template<typename T, int Rows>
  using Vector = Matrix<T, Rows, 1>;
  export template<typename T>
  using Vector1 = Vector<T, 1>;
  export using Vector1f = Vector1<float>;
  export using Vector1i = Vector1<std::int32_t>;
  export using Vector1u = Vector1<std::uint32_t>;
  export template<typename T>
  using Vector2 = Vector<T, 2>;
  export using Vector2f = Vector2<float>;
  export using Vector2i = Vector2<std::int32_t>;
  export using Vector2u = Vector2<std::uint32_t>;
  export template<typename T>
  using Vector3 = Vector<T, 3>;
  export using Vector3f = Vector3<float>;
  export using Vector3i = Vector3<std::int32_t>;
  export using Vector3u = Vector3<std::uint32_t>;
  export template<typename T>
  using Vector4 = Vector<T, 4>;
  export using Vector4f = Vector4<float>;
  export using Vector4i = Vector4<std::int32_t>;
  export using Vector4u = Vector4<std::uint32_t>;
  export template<typename T>
  using VectorX = Vector<T, Eigen::Dynamic>;
  export using VectorXf = VectorX<float>;
  export using VectorXi = VectorX<std::int32_t>;
  export using VectorXu = VectorX<std::uint32_t>;
}