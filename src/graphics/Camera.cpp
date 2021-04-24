#include "Camera.h"

namespace imp {
  Camera::Camera(CameraCreateInfo const &createInfo) noexcept:
      transform_{createInfo.transform},
      tanHalfFovX_{createInfo.tanHalfFovX},
      tanHalfFovY_{createInfo.tanHalfFovY},
      minDepth_{createInfo.minDepth},
      maxDepth_{createInfo.maxDepth} {}

  Transform const &Camera::getTransform() const noexcept {
    return transform_;
  }

  float Camera::getTanHalfFovX() const noexcept {
    return tanHalfFovX_;
  }

  float Camera::getTanHalfFovY() const noexcept {
    return tanHalfFovY_;
  }

  float Camera::getMinDepth() const noexcept {
    return minDepth_;
  }

  float Camera::getMaxDepth() const noexcept {
    return maxDepth_;
  }

  Matrix4x4f Camera::getViewMatrix() const noexcept {
    auto ret = rotationMatrix4x4(conjugate(transform_.getRotation()));
    ret[3] = ret * concatenate(-transform_.getTranslation(), 1.0f);
    return ret;
  }

  Matrix4x4f Camera::getProjectionMatrix() const noexcept {
    return perspectiveMatrix(
        -tanHalfFovX_,
        tanHalfFovX_,
        tanHalfFovY_,
        -tanHalfFovY_,
        minDepth_,
        maxDepth_);
  }

  void Camera::setTransform(Transform const &transform) noexcept {
    transform_ = transform;
  }

  void Camera::setTanHalfFovX(float tanHalfFovX) noexcept {
    tanHalfFovX_ = tanHalfFovX;
  }

  void Camera::setTanHalfFovY(float tanHalfFovY) noexcept {
    tanHalfFovY_ = tanHalfFovY;
  }

  void Camera::setMinDepth(float minDepth) noexcept {
    minDepth_ = minDepth;
  }

  void Camera::setMaxDepth(float maxDepth) noexcept {
    maxDepth_ = maxDepth;
  }

  void Camera::translate(Vector3f const &v) noexcept {
    transform_.translate(v);
  }

  void Camera::rotateLocal(Quaternionf const &q) noexcept {
    transform_.rotateLocal(q);
  }

  void Camera::rotateGlobal(Quaternionf const &q) noexcept {
    transform_.rotateGlobal(q);
  }
} // namespace imp