#include "Widget.h"

namespace imp {
  unsigned Widget::getWidth() const noexcept {
    return width_;
  }

  unsigned Widget::getHeight() const noexcept {
    return height_;
  }

  unsigned Widget::getPreferredWidth() const noexcept {
    return preferredWidth_;
  }

  void Widget::setPreferredWidth(unsigned width) noexcept {
    preferredWidth_ = width;
  }

  unsigned Widget::getPreferredHeight() const noexcept {
    return preferredHeight_;
  }

  void Widget::setPreferredHeight(unsigned height) noexcept {
    preferredHeight_ = height;
  }

  Eigen::Vector2i const &Widget::getTranslation() const noexcept {
    return translation_;
  }

  void Widget::setTranslation(Eigen::Vector2i const &translation) noexcept {
    translation_ = translation;
  }

  void Widget::setWidth(unsigned width) noexcept {
    width_ = width;
  }

  void Widget::setHeight(unsigned height) noexcept {
    height_ = height;
  }
} // namespace imp