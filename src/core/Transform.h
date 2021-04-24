#pragma once

#include "../math/Matrix.h"

namespace imp {
  class Transform {
  public:
    Transform(
        Vector3f const &translation = zeroVector<3, float>(),
        Quaternionf const &rotation = identityQuaternion<float>()) noexcept;

    Vector3f const &getTranslation() const noexcept;
    Quaternionf const &getRotation() const noexcept;
    Matrix4x4f getMatrix() const noexcept;

    void setTranslation(Vector3f const &translation) noexcept;
    void setRotation(Quaternionf const &rotation) noexcept;

    void translate(Vector3f const &v) noexcept;
    void rotateLocal(Quaternionf const &q) noexcept;
    void rotateGlobal(Quaternionf const &q) noexcept;

  private:
    Vector3f translation_;
    Quaternionf rotation_;
  };
} // namespace imp