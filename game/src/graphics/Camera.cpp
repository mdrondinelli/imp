//#include "Camera.h"
//
//namespace imp {
//  Camera::Camera(
//      Eigen::Matrix4f const &viewMatrix,
//      Eigen::Matrix4f const &projectionMatrix,
//      Extent2u const &extent) noexcept:
//      viewMatrix_{viewMatrix},
//      projectionMatrix_{projectionMatrix},
//      extent_{extent} {}
//
//  Eigen::Matrix4f const &Camera::getViewMatrix() const noexcept {
//    return viewMatrix_;
//  }
//
//  void Camera::setViewMatrix(Eigen::Matrix4f const& viewMatrix) noexcept {
//    viewMatrix_ = viewMatrix;
//  }
//
//  Eigen::Matrix4f const &Camera::getProjectionMatrix() const noexcept {
//    return projectionMatrix_;
//  }
//
//  void Camera::setProjectionMatrix(
//    Eigen::Matrix4f const& projectionMatrix) noexcept {
//    projectionMatrix_ = projectionMatrix;
//  }
//
//  Extent2u const &Camera::getExtent() const noexcept {
//    return extent_;
//  }
//
//  void Camera::setExtent(Extent2u const& extent) noexcept {
//    extent_ = extent;
//  }
//
//  //Eigen::Matrix4f getViewMatrix(Camera const &c) noexcept {
//  //  return (c.getOrientation().conjugate() *
//  //          Eigen::Translation3f{-c.getPosition()})
//  //      .matrix();
//  //}
//
//  //Eigen::Matrix4f getProjectionMatrix(Camera const &c) noexcept {
//  //  auto w = 2 * c.getTanHalfFovX();
//  //  auto h = 2 * c.getTanHalfFovY();
//  //  auto n = c.getMinDepth();
//  //  auto f = c.getMaxDepth();
//  //  auto m = Eigen::Matrix4f::Zero().eval();
//  //  m(0, 0) = 2 * n / w;
//  //  m(1, 1) = 2 * n / -h;
//  //  m(2, 2) = n / (f - n);
//  //  m(2, 3) = f * n / (f - n);
//  //  m(3, 2) = -1;
//  //  return m;
//  //}
//} // namespace imp