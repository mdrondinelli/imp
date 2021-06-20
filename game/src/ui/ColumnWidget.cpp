#include "ColumnWidget.h"

#include <algorithm>

namespace imp {
  gsl::span<std::pair<std::shared_ptr<Widget>, float> const>
  ColumnWidget::getWidgets() const noexcept {
    return widgets_;
  }

  void
  ColumnWidget::pushFront(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(flexibility >= 0.0f);
    widgets_.emplace(widgets_.begin(), std::move(widget), flexibility);
  }

  void
  ColumnWidget::pushBack(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(flexibility >= 0.0f);
    widgets_.emplace_back(std::move(widget), flexibility);
  }

  void ColumnWidget::insert(
      std::size_t index, std::shared_ptr<Widget> widget, float flexibility) {
    Expects(index <= widgets_.size());
    Expects(flexibility >= 0.0f);
    widgets_.emplace(widgets_.begin() + index, std::move(widget), flexibility);
  }

  void ColumnWidget::popFront() {
    Expects(!widgets_.empty());
    widgets_.erase(widgets_.begin());
  }

  void ColumnWidget::popBack() {
    Expects(!widgets_.empty());
    widgets_.pop_back();
  }

  void ColumnWidget::erase(std::size_t index) {
    Expects(index <= widgets_.size());
    widgets_.erase(widgets_.begin() + index);
  }

  unsigned ColumnWidget::getMinWidth() const {
    auto minWidth = 0u;
    for (auto const &[widget, _] : widgets_) {
      if (widget) {
        minWidth = std::max(minWidth, widget->getMinWidth());
      }
    }
    return minWidth;
  }

  unsigned ColumnWidget::getMinHeight() const {
    auto flexibilityScale = 0.0f;
    auto flexibilitySum = 0.0f;
    auto rigidHeight = 0u;
    for (auto const &[widget, flexibility] : widgets_) {
      if (widget) {
        if (flexibility) {
          auto minFlexibilityScale = widget->getMinHeight() / flexibility;
          flexibilityScale = std::max(flexibilityScale, minFlexibilityScale);
        } else {
          rigidHeight += widget->getMinHeight();
        }
      }
      flexibilitySum += flexibility;
    }
    auto minFlexibleHeight =
        static_cast<unsigned>(std::round(flexibilityScale * flexibilitySum));
    return rigidHeight + minFlexibleHeight;
  }

  void ColumnWidget::layout() {
    auto width = getPreferredWidth();
    auto rigidHeight = 0u;
    auto flexibilityScale = 0.0f;
    auto flexibilitySum = 0.0f;
    for (auto const &[widget, flexibility] : widgets_) {
      if (widget) {
        width = std::max(width, widget->getMinWidth());
        if (flexibility) {
          auto minFlexibilityScale = widget->getMinHeight() / flexibility;
          flexibilityScale = std::max(flexibilityScale, minFlexibilityScale);
        } else {
          rigidHeight += widget->getMinHeight();
        }
      }
      flexibilitySum += flexibility;
    }
    auto minFlexibleHeight =
        static_cast<unsigned>(std::round(flexibilityScale * flexibilitySum));
    auto minHeight = rigidHeight + minFlexibleHeight;
    auto height = std::max(getPreferredHeight(), minHeight);
    setWidth(width);
    setHeight(height);
    flexibilityScale = (height - rigidHeight) / flexibilitySum;
    auto translation = 0.0f;
    auto roundedTranslation = 0;
    for (auto const &[widget, flexibility] : widgets_) {
      if (widget) {
        widget->setTranslation({0, roundedTranslation});
        widget->setPreferredWidth(width);
        if (flexibility) {
          auto prevRoundedTranslation = roundedTranslation;
          translation += flexibilityScale * flexibility;
          roundedTranslation = static_cast<int>(std::round(translation));
          widget->setPreferredHeight(
              roundedTranslation - prevRoundedTranslation);
          widget->layout();
        } else {
          widget->setPreferredHeight(0u);
          widget->layout();
          translation += widget->getHeight();
          roundedTranslation += widget->getHeight();
        }
      } else if (flexibility) {
        translation += flexibilityScale * flexibility;
        roundedTranslation = static_cast<int>(std::round(translation));
      }
    }
  }
} // namespace imp