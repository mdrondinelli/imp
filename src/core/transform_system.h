#pragma once

#include <entt/entt.hpp>

#include "../math/matrix.h"

namespace imp {
  class transform_system {
  public:
    /**
     * @brief Constructs a transform_system.
     */
    transform_system(entt::registry *registry);
    transform_system(transform_system const &rhs) = delete;
    transform_system &operator=(transform_system &rhs) = delete;

    /**
     * @brief Attaches child to parent.
     */
    void attach(entt::entity child, entt::entity parent);

    /**
     * @brief Detaches child from its parent.
     */
    void detach(entt::entity child);

    /**
     * @returns the translation of entity e.
     */
    vector3f const &translation(entt::entity e) const;

    /**
     * @returns the rotation of entity e.
     */
    quaternionf const &rotation(entt::entity e) const;

    /**
     * @returns the matrix of entity e.
     */
    matrix4x4f const &matrix(entt::entity e);

    /**
     * @brief Translates entity e by vector v.
     */
    void translate(entt::entity e, vector3f const &v);

    /**
     * @brief Pre-rotates entity e by quaternion q.
     */
    void pre_rotate(entt::entity e, quaternionf const &q);

    /**
     * @brief Post-rotates entity e by quaternion q.
     */
    void post_rotate(entt::entity e, quaternionf const &q);

    /**
     * @brief sets the translation of entity e.
     */
    void set_translation(entt::entity e, vector3f const &v);

    /**
     * @brief sets the rotation of entity e.
     */
    void set_rotation(entt::entity e, quaternionf const &q);

  private:
    entt::registry *registry_;

    void invalidate_matrix(entt::entity e);
  };
} // namespace imp