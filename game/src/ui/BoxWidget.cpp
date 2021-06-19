#include "BoxWidget.h"

namespace imp {
  BoxWidget::BoxWidget(
      unsigned minWidth,
      unsigned minHeight) noexcept:
      minWidth_{minWidth},
      minHeight_{minHeight} {}

  unsigned BoxWidget::getMinWidth() const {
    return minWidth_;
  }

  void BoxWidget::setMinWidth(unsigned minWidth) noexcept {
    minWidth_ = minWidth;
  }

  unsigned BoxWidget::getMinHeight() const {
    return minHeight_;
  }

  void BoxWidget::setMinHeight(unsigned minHeight) noexcept {
    minHeight_ = minHeight;
  }

  void BoxWidget::layout() {
    if (auto preferredWidth = getPreferredWidth()) {
      setWidth(std::max(*preferredWidth, minWidth_));
    } else {
      setWidth(minWidth_);
    }
    if (auto preferredHeight = getPreferredHeight()) {
      setHeight(std::max(*preferredHeight, minHeight_));
    } else {
      setHeight(minHeight_);
    }
  }
} // namespace imp