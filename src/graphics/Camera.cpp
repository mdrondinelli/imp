#include "Camera.h"

namespace imp {
  Camera::Camera(
      Eigen::Vector3f const &position,
      Eigen::Quaternionf const &orientation,
      float tanHalfFovX,
      float tanHalfFovY,
      float minDepth,
      float maxDepth) noexcept:
      position_{position},
      orientation_{orientation},
      tanHalfFovX_{tanHalfFovX},
      tanHalfFovY_{tanHalfFovY},
      minDepth_{minDepth},
      maxDepth_{maxDepth} {}

  Eigen::Vector3f const &Camera::getPosition() const noexcept {
    return position_;
  }

  Eigen::Quaternionf const &Camera::getOrientation() const noexcept {
    return orientation_;
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

  // Matrix4x4f Camera::getViewMatrix() const noexcept {
  //  auto ret = rotationMatrix4x4(conjugate(transform_.getRotation()));
  //  ret[3] = ret * concatenate(-transform_.getTranslation(), 1.0f);
  //  return ret;
  //}

  // Matrix4x4f Camera::getProjectionMatrix() const noexcept {
  //  return perspectiveMatrix(
  //      -tanHalfFovX_,
  //      tanHalfFovX_,
  //      tanHalfFovY_,
  //      -tanHalfFovY_,
  //      minDepth_,
  //      maxDepth_);
  //}

  void Camera::setPosition(Eigen::Vector3f const &position) noexcept {
    position_ = position;
  }

  void Camera::setOrientation(Eigen::Quaternionf const &orientation) noexcept {
    orientation_ = orientation;
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

  void Camera::translate(Eigen::Vector3f const &v) noexcept {
    position_ += v;
  }

  void Camera::rotateLocal(Eigen::Quaternionf const &q) noexcept {
    orientation_ *= q;
  }

  void Camera::rotateGlobal(Eigen::Quaternionf const &q) noexcept {
    orientation_ = q * orientation_;
  }

  Eigen::Matrix4f getViewMatrix(Camera const &c) noexcept {
    return (c.getOrientation().conjugate() *
            Eigen::Translation3f{-c.getPosition()})
        .matrix();
  }

  Eigen::Matrix4f getProjectionMatrix(Camera const &c) noexcept {
    auto w = 2 * c.getTanHalfFovX();
    auto h = 2 * c.getTanHalfFovY();
    auto n = c.getMinDepth();
    auto f = c.getMaxDepth();
    auto m = Eigen::Matrix4f::Zero().eval();
    m(0, 0) = 2 * n / w;
    m(1, 1) = 2 * n / -h;
    m(2, 2) = n / (f - n);
    m(2, 3) = f * n / (f - n);
    m(3, 2) = -1;
    return m;
  }
} // namespace imp