#pragma once

#include <optional>

#include <Eigen/Dense>

namespace imp {
  class Widget {
  public:
    virtual ~Widget() = default;
    virtual unsigned getMinWidth() const = 0;
    virtual unsigned getMinHeight() const = 0;
    virtual void layout() = 0;

    unsigned getWidth() const noexcept;
    unsigned getHeight() const noexcept;
    
    std::optional<unsigned> getPreferredWidth() const noexcept;
    void setPreferredWidth(std::optional<unsigned> width) noexcept;

    std::optional<unsigned> getPreferredHeight() const noexcept;
    void setPreferredHeight(std::optional<unsigned> height) noexcept;
    
    Eigen::Vector2i const &getTranslation() const noexcept;
    void setTranslation(Eigen::Vector2i const &translation) noexcept;

  protected:
    void setWidth(unsigned width) noexcept;
    void setHeight(unsigned height) noexcept;

  private:
    unsigned width_;
    unsigned height_;
    std::optional<unsigned> preferredWidth_;
    std::optional<unsigned> preferredHeight_;
    Eigen::Vector2i translation_;
  };
} // namespace imp