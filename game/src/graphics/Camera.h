//#pragma once
//
//#include <Eigen/Dense>
//
//#include "../util/Extent.h"
//
//namespace imp {
//  class Camera {
//  public:
//    explicit Camera(
//        Eigen::Matrix4f const &viewMatrix,
//        Eigen::Matrix4f const &projectionMatrix,
//        Extent2u const &extent) noexcept;
//
//    Eigen::Matrix4f const &getViewMatrix() const noexcept;
//    void setViewMatrix(Eigen::Matrix4f const &viewMatrix) noexcept;
//
//    Eigen::Matrix4f const &getProjectionMatrix() const noexcept;
//    void setProjectionMatrix(Eigen::Matrix4f const &projectionMatrix) noexcept;
//
//    Extent2u const &getExtent() const noexcept;
//    void setExtent(Extent2u const &extent) noexcept;
//
//  private:
//    Eigen::Matrix4f viewMatrix_;
//    Eigen::Matrix4f projectionMatrix_;
//    Extent2u extent_;
//  };
//} // namespace imp