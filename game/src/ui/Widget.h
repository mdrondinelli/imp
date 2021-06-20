#pragma once

#include <optional>

#include <Eigen/Dense>

namespace imp {
  class Widget {
  public:
    virtual ~Widget() = default;

    virtual unsigned getMinWidth() const = 0;
    virtual unsigned getMinHeight() const = 0;

    unsigned getPreferredWidth() const noexcept;
    void setPreferredWidth(unsigned width) noexcept;

    unsigned getPreferredHeight() const noexcept;
    void setPreferredHeight(unsigned height) noexcept;
    
    unsigned getWidth() const noexcept;
    unsigned getHeight() const noexcept;
    
    Eigen::Vector2i const &getTranslation() const noexcept;
    void setTranslation(Eigen::Vector2i const &translation) noexcept;

    virtual void layout() = 0;

  protected:
    void setWidth(unsigned width) noexcept;
    void setHeight(unsigned height) noexcept;

  private:
    unsigned preferredWidth_{0u};
    unsigned preferredHeight_{0u};
    unsigned width_{0u};
    unsigned height_{0u};
    Eigen::Vector2i translation_{0, 0};
  };
} // namespace imp