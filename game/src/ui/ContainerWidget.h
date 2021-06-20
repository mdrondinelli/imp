#pragma once

#include <memory>

#include "Widget.h"

namespace imp {
  enum class Alignment { NEGATIVE, CENTER, POSITIVE };

  class ContainerWidget: public Widget {
  public:
    ContainerWidget() noexcept = default;
    explicit ContainerWidget(
        std::shared_ptr<Widget> content,
        Alignment horizontalAlignment = Alignment::NEGATIVE,
        Alignment verticalAlignment = Alignment::NEGATIVE,
        unsigned minWidth = 0u,
        unsigned minHeight = 0u) noexcept;

    void layout() override;

    std::shared_ptr<Widget> getContent() const noexcept;
    void setContent(std::shared_ptr<Widget> content) noexcept;

    Alignment getHorizontalAlignment() const noexcept;
    void setHorizontalAlignment(Alignment alignment) noexcept;

    Alignment getVerticalAligment() const noexcept;
    void setVerticalAlignment(Alignment alignment) noexcept;

    unsigned getMinWidth() const override;
    void setMinWidth(unsigned minWidth) noexcept;

    unsigned getMinHeight() const override;
    void setMinHeight(unsigned minHeight) noexcept;

  private:
    std::shared_ptr<Widget> content_;
    Alignment horizontalAlignment_ = Alignment::NEGATIVE;
    Alignment verticalAlignment_ = Alignment::NEGATIVE;
    unsigned minWidth_ = 0u;
    unsigned minHeight_ = 0u;
  };
} // namespace imp