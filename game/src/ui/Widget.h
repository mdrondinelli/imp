#pragma once

#include <optional>

#include <Eigen/Dense>

namespace imp {
  class Widget {
  public:
    virtual ~Widget() = default;

    unsigned getWidth() const noexcept;
    unsigned getHeight() const noexcept;

    virtual unsigned getMinWidth() const = 0;
    virtual unsigned getMinHeight() const = 0;
    
    std::optional<unsigned> getPreferredWidth() const noexcept;
    void setPreferredWidth(std::optional<unsigned> width) noexcept;

    std::optional<unsigned> getPreferredHeight() const noexcept;
    void setPreferredHeight(std::optional<unsigned> height) noexcept;
    
    Eigen::Vector2i const &getTranslation() const noexcept;
    void setTranslation(Eigen::Vector2i const &translation) noexcept;

    virtual void layout() = 0;

  protected:
    void setWidth(unsigned width) noexcept;
    void setHeight(unsigned height) noexcept;

  private:
    unsigned width_{0u};
    unsigned height_{0u};
    std::optional<unsigned> preferredWidth_;
    std::optional<unsigned> preferredHeight_;
    Eigen::Vector2i translation_{0, 0};
  };
} // namespace imp