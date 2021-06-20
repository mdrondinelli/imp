#include "RowWidget.h"

#include <algorithm>

namespace imp {
  gsl::span<std::pair<std::shared_ptr<Widget>, float> const>
  RowWidget::getWidgets() const noexcept {
    return widgets_;
  }

  void RowWidget::pushFront(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(flexibility >= 0.0f);
    widgets_.emplace(widgets_.begin(), std::move(widget), flexibility);
  }

  void RowWidget::pushBack(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(flexibility >= 0.0f);
    widgets_.emplace_back(std::move(widget), flexibility);
  }

  void RowWidget::insert(
      std::size_t index, std::shared_ptr<Widget> widget, float flexibility) {
    Expects(index <= widgets_.size());
    Expects(flexibility >= 0.0f);
    widgets_.emplace(widgets_.begin() + index, std::move(widget), flexibility);
  }

  void RowWidget::popFront() {
    Expects(!widgets_.empty());
    widgets_.erase(widgets_.begin());
  }

  void RowWidget::popBack() {
    Expects(!widgets_.empty());
    widgets_.pop_back();
  }

  void RowWidget::erase(std::size_t index) {
    Expects(index <= widgets_.size());
    widgets_.erase(widgets_.begin() + index);
  }

  unsigned RowWidget::getMinWidth() const {
    auto flexibilityScale = 0.0f;
    auto flexibilitySum = 0.0f;
    auto rigidWidth = 0u;
    for (auto const &[widget, flexibility] : widgets_) {
      if (widget) {
        if (flexibility) {
          auto minFlexibilityScale = widget->getMinWidth() / flexibility;
          flexibilityScale = std::max(flexibilityScale, minFlexibilityScale);
        } else {
          rigidWidth += widget->getMinWidth();
        }
      }
      flexibilitySum += flexibility;
    }
    auto flexibleWidth =
        static_cast<unsigned>(std::round(flexibilityScale * flexibilitySum));
    return rigidWidth + flexibleWidth;
  }

  unsigned RowWidget::getMinHeight() const {
    auto minHeight = 0u;
    for (auto const &[widget, _] : widgets_) {
      if (widget) {
        minHeight = std::max(minHeight, widget->getMinHeight());
      }
    }
    return minHeight;
  }

  void RowWidget::layout() {
    auto height = getPreferredHeight();
    auto flexibilityScale = 0.0f;
    auto flexibilitySum = 0.0f;
    auto rigidWidth = 0u;
    for (auto const &[widget, flexibility] : widgets_) {
      if (widget) {
        height = std::max(height, widget->getMinHeight());
        if (flexibility) {
          auto minFlexibilityScale = widget->getMinWidth() / flexibility;
          flexibilityScale = std::max(flexibilityScale, minFlexibilityScale);
        } else {
          rigidWidth += widget->getMinWidth();
        }
      }
      flexibilitySum += flexibility;
    }
    auto minFlexibleWidth =
        static_cast<unsigned>(std::round(flexibilityScale * flexibilitySum));
    auto minWidth = rigidWidth + minFlexibleWidth;
    auto width = std::max(getPreferredWidth(), minWidth);
    setWidth(width);
    setHeight(height);
    flexibilityScale = (width - rigidWidth) / flexibilitySum;
    auto translation = 0.0f;
    auto roundedTranslation = 0;
    for (auto const &[widget, flexibility] : widgets_) {
      if (widget) {
        widget->setTranslation({roundedTranslation, 0});
        widget->setPreferredHeight(height);
        if (flexibility) {
          auto prevRoundedTranslation = roundedTranslation;
          translation += flexibilityScale * flexibility;
          roundedTranslation = static_cast<int>(std::round(translation));
          widget->setPreferredWidth(
              roundedTranslation - prevRoundedTranslation);
          widget->layout();
        } else {
          widget->setPreferredWidth(0u);
          widget->layout();
          translation += widget->getWidth();
          roundedTranslation += widget->getWidth();
        }
      } else if (flexibility) {
        translation += flexibilityScale * flexibility;
        roundedTranslation = static_cast<int>(std::round(translation));
      }
    }
  }
} // namespace imp