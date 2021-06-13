#pragma once

#include <Eigen/Dense>

namespace imp {
  class Camera {
  public:
    Camera(
        Eigen::Vector3f const &position = Eigen::Vector3f::Zero(),
        Eigen::Quaternionf const &orientation = Eigen::Quaternionf::Identity(),
        float tanHalfFovX = 1.0f,
        float tanHalfFovY = 1.0f,
        float minDepth = 1.0f,
        float maxDepth = 1024.0f) noexcept;

    Eigen::Vector3f const &getPosition() const noexcept;
    Eigen::Quaternionf const &getOrientation() const noexcept;
    float getTanHalfFovX() const noexcept;
    float getTanHalfFovY() const noexcept;
    float getMinDepth() const noexcept;
    float getMaxDepth() const noexcept;

    void setPosition(Eigen::Vector3f const &position) noexcept;
    void setOrientation(Eigen::Quaternionf const &orientation) noexcept;
    void setTanHalfFovX(float tanHalfFovX) noexcept;
    void setTanHalfFovY(float tanHalfFovY) noexcept;
    void setMinDepth(float minDepth) noexcept;
    void setMaxDepth(float maxDepth) noexcept;

    void translate(Eigen::Vector3f const &v) noexcept;
    void rotateLocal(Eigen::Quaternionf const &q) noexcept;
    void rotateGlobal(Eigen::Quaternionf const &q) noexcept;

  private:
    Eigen::Vector3f position_;
    Eigen::Quaternionf orientation_;
    float tanHalfFovX_;
    float tanHalfFovY_;
    float minDepth_;
    float maxDepth_;
  };

  Eigen::Matrix4f getViewMatrix(Camera const &c) noexcept;
  Eigen::Matrix4f getProjectionMatrix(Camera const &c) noexcept;
} // namespace imp