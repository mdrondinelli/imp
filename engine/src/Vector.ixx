// clang-format off
module;
#include <Eigen/Dense>
export module mobula.engine.util:Vector;
import :Matrix;
// clang-format on

namespace mobula {
  export template<typename Scalar, int Rows>
  using Vector = Matrix<Scalar, Rows, 1>;
  export using Vector1f = Vector<float, 1>;
  export using Vector2f = Vector<float, 2>;
  export using Vector3f = Vector<float, 3>;
  export using Vector4f = Vector<float, 4>;
  export using VectorXf = Vector<float, Eigen::Dynamic>;
  export using Vector1i = Vector<std::int32_t, 1>;
  export using Vector2i = Vector<std::int32_t, 2>;
  export using Vector3i = Vector<std::int32_t, 3>;
  export using Vector4i = Vector<std::int32_t, 4>;
  export using VectorXi = Vector<std::int32_t, Eigen::Dynamic>;
  export using Vector1u = Vector<std::uint32_t, 1>;
  export using Vector2u = Vector<std::uint32_t, 2>;
  export using Vector3u = Vector<std::uint32_t, 3>;
  export using Vector4u = Vector<std::uint32_t, 4>;
  export using VectorXu = Vector<std::uint32_t, Eigen::Dynamic>;
}