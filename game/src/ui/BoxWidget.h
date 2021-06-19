#pragma once

#include "Widget.h"

namespace imp {
  class BoxWidget: public Widget {
  public:
    explicit BoxWidget(
        unsigned minWidth,
        unsigned minHeight) noexcept;

    unsigned getMinWidth() const override;
    void setMinWidth(unsigned minWidth) noexcept;

    unsigned getMinHeight() const override;
    void setMinHeight(unsigned minHeight) noexcept;

    void layout() override;

  private:
    unsigned minWidth_;
    unsigned minHeight_;
  };
} // namespace imp