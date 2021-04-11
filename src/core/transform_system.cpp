#include "transform_system.h"

#include "transform_component.h"

namespace imp {
  transform_system::transform_system(entt::registry *registry):
      registry_{registry} {}

  void transform_system::attach(entt::entity child, entt::entity parent) {
    auto &child_component = registry_->get<transform_component>(child);
    if (child_component.parent != parent) {
      detach(child);
      auto &parent_component = registry_->get<transform_component>(parent);
      auto pos = parent_component.children.size() > 32
                     ? std::lower_bound(
                           parent_component.children.begin(),
                           parent_component.children.end(),
                           child)
                     : std::find_if(
                           parent_component.children.begin(),
                           parent_component.children.end(),
                           [=](auto c) { return c >= child; });
      parent_component.children.emplace(pos, child);
      child_component.parent = parent;
      invalidate_matrix(child);
    }
  }

  void transform_system::detach(entt::entity child) {
    auto &child_component = registry_->get<transform_component>(child);
    if (child_component.parent != entt::null) {
      auto &parent_component =
          registry_->get<transform_component>(child_component.parent);
      child_component.parent = entt::null;
      if (parent_component.children.size() > 32) {
        auto pos = std::lower_bound(
            parent_component.children.begin(),
            parent_component.children.end(),
            child);
        if (pos != parent_component.children.end() && *pos == child)
          parent_component.children.erase(pos);
      } else {
        auto pos = std::find(
            parent_component.children.begin(),
            parent_component.children.end(),
            child);
        if (pos != parent_component.children.end())
          parent_component.children.erase(pos);
      }
      invalidate_matrix(child);
    }
  }

  vector3f const &transform_system::translation(entt::entity e) const {
    return registry_->get<transform_component>(e).translation;
  }

  quaternionf const &transform_system::rotation(entt::entity e) const {
    return registry_->get<transform_component>(e).rotation;
  }

  matrix4x4f const &transform_system::matrix(entt::entity e) {
    auto &component = registry_->get<transform_component>(e);
    if (!component.matrix_valid) {
      component.matrix = rotation_matrix4x4(component.rotation);
      component.matrix[3] = concatenate(component.translation, 1.0f);
      if (component.parent != entt::null)
        component.matrix = matrix(component.parent) * component.matrix;
      component.matrix_valid = true;
    }
    return component.matrix;
  }

  void transform_system::translate(entt::entity e, vector3f const &v) {
    auto &component = registry_->get<transform_component>(e);
    component.translation += v;
    invalidate_matrix(e);
  }

  void transform_system::pre_rotate(entt::entity e, quaternionf const &q) {
    auto &component = registry_->get<transform_component>(e);
    component.rotation *= q;
    invalidate_matrix(e);
  }

  void transform_system::post_rotate(entt::entity e, quaternionf const &q) {
    auto &component = registry_->get<transform_component>(e);
    component.rotation = q * component.rotation;
    invalidate_matrix(e);
  }

  void transform_system::set_translation(entt::entity e, vector3f const &v) {
    auto &component = registry_->get<transform_component>(e);
    component.translation = v;
    invalidate_matrix(e);
  }

  void transform_system::set_rotation(entt::entity e, quaternionf const &q) {
    auto &component = registry_->get<transform_component>(e);
    component.rotation = q;
    invalidate_matrix(e);
  }

  void transform_system::invalidate_matrix(entt::entity e) {
    auto &component = registry_->get<transform_component>(e);
    if (component.matrix_valid) {
      component.matrix_valid = false;
      for (auto &child : component.children)
        invalidate_matrix(e);
    }
  }
} // namespace imp