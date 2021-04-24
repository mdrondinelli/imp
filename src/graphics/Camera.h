#pragma once

#include "../core/Transform.h"

namespace imp {
  struct CameraCreateInfo {
    Transform transform;
    float tanHalfFovX = 1.0f;
    float tanHalfFovY = 1.0f;
    float minDepth = 0.25f;
    float maxDepth = 256.0f;
  };

  class Camera {
  public:
    explicit Camera(CameraCreateInfo const &createInfo) noexcept;

    Transform const &getTransform() const noexcept;
    float getTanHalfFovX() const noexcept;
    float getTanHalfFovY() const noexcept;
    float getMinDepth() const noexcept;
    float getMaxDepth() const noexcept;
    Matrix4x4f getViewMatrix() const noexcept;
    Matrix4x4f getProjectionMatrix() const noexcept;

    void setTransform(Transform const &transform) noexcept;
    void setTanHalfFovX(float tanHalfFovX) noexcept;
    void setTanHalfFovY(float tanHalfFovU) noexcept;
    void setMinDepth(float minDepth) noexcept;
    void setMaxDepth(float maxDepth) noexcept;

    void translate(Vector3f const &v) noexcept;
    void rotateLocal(Quaternionf const &q) noexcept;
    void rotateGlobal(Quaternionf const &q) noexcept;

  private:
    Transform transform_;
    float tanHalfFovX_;
    float tanHalfFovY_;
    float minDepth_;
    float maxDepth_;
  };
} // namespace imp