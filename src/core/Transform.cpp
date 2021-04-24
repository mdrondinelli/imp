#include "Transform.h"

namespace imp {
  Transform::Transform(
      Vector3f const &translation, Quaternionf const &rotation) noexcept:
      translation_{translation}, rotation_{rotation} {}

  Vector3f const &Transform::getTranslation() const noexcept {
    return translation_;
  }

  Quaternionf const &Transform::getRotation() const noexcept {
    return rotation_;
  }

  Matrix4x4f Transform::getMatrix() const noexcept {
    auto ret = rotationMatrix4x4(rotation_);
    ret[3][0] = translation_[0];
    ret[3][1] = translation_[1];
    ret[3][2] = translation_[2];
    return ret;
  }

  void Transform::setTranslation(Vector3f const &translation) noexcept {
    translation_ = translation;
  }

  void Transform::setRotation(Quaternionf const &rotation) noexcept {
    rotation_ = rotation;
  }

  void Transform::translate(Vector3f const &v) noexcept {
    translation_ += v;
  }

  void Transform::rotateLocal(Quaternionf const &q) noexcept {
    rotation_ *= q;
  }

  void Transform::rotateGlobal(Quaternionf const &q) noexcept {
    rotation_ = q * rotation_;
  }
} // namespace imp