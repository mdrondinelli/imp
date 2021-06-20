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
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void
  ColumnWidget::pushBack(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(flexibility >= 0.0f);
    widgets_.emplace_back(std::move(widget), flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void ColumnWidget::insert(
      std::size_t index, std::shared_ptr<Widget> widget, float flexibility) {
    Expects(index <= widgets_.size());
    Expects(flexibility >= 0.0f);
    widgets_.emplace(widgets_.begin() + index, std::move(widget), flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void ColumnWidget::popFront() {
    Expects(!widgets_.empty());
    if (widgets_.front().second) {
      --flexibleWidgetCount_;
    }
    widgets_.erase(widgets_.begin());
  }

  void ColumnWidget::popBack() {
    Expects(!widgets_.empty());
    if (widgets_.back().second) {
      --flexibleWidgetCount_;
    }
    widgets_.pop_back();
  }

  void ColumnWidget::erase(std::size_t index) {
    Expects(index <= widgets_.size());
    if (widgets_[index].second) {
      --flexibleWidgetCount_;
    }
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
          flexibilityScale =
              std::max(flexibilityScale, widget->getMinHeight() / flexibility);
        } else {
          rigidHeight += widget->getMinHeight();
        }
      }
      flexibilitySum += flexibility;
    }
    auto flexibleHeight =
        static_cast<unsigned>(std::round(flexibilityScale * flexibilitySum));
    return rigidHeight + flexibleHeight;
  }

  void ColumnWidget::layout() {
    auto preferredWidth = getPreferredWidth();
    auto width = preferredWidth ? std::max(*preferredWidth, getMinWidth())
                                : getMinWidth();
    setWidth(width);
    if (flexibleWidgetCount_) {
      auto flexibilityScale = 0.0f;
      auto flexibilitySum = 0.0f;
      auto rigidHeight = 0u;
      for (auto const &[widget, flexibility] : widgets_) {
        if (widget) {
          if (flexibility) {
            flexibilityScale = std::max(
                flexibilityScale, widget->getMinHeight() / flexibility);
          } else {
            rigidHeight += widget->getMinHeight();
          }
        }
        flexibilitySum += flexibility;
      }
      if (auto preferredTotalHeight = getPreferredHeight()) {
        auto minFlexibleHeight = static_cast<unsigned>(
            std::round(flexibilityScale * flexibilitySum));
        auto minTotalHeight = rigidHeight + minFlexibleHeight;
        auto totalHeight = std::max(*preferredTotalHeight, minTotalHeight);
        auto flexibleHeight = totalHeight - rigidHeight;
        flexibilityScale = flexibleHeight / flexibilitySum;
      }
      auto translation = 0.0f;
      for (auto const &[widget, flexibility] : widgets_) {
        if (widget) {
          if (flexibility) {
            auto minY = static_cast<unsigned>(std::round(translation));
            translation += flexibilityScale * flexibility;
            auto maxY = static_cast<unsigned>(std::round(translation));
            widget->setPreferredWidth(width);
            widget->setPreferredHeight(maxY - minY);
            widget->setTranslation({0, minY});
            widget->layout();
          } else {
            widget->setPreferredWidth(width);
            widget->setPreferredHeight(std::nullopt);
            widget->setTranslation({0, std::round(translation)});
            widget->layout();
            translation += widget->getHeight();
          }
        } else {
          translation += flexibilityScale * flexibility;
        }
      }
      setHeight(static_cast<unsigned>(std::round(translation)));
    } else {
      auto minHeight = 0u;
      for (auto const &[widget, _] : widgets_) {
        widget->setPreferredWidth(width);
        widget->setPreferredHeight(std::nullopt);
        widget->setTranslation({0, minHeight});
        widget->layout();
        minHeight += widget->getHeight();
      }
      auto preferredHeight = getPreferredHeight();
      auto height =
          preferredHeight ? std::max(*preferredHeight, minHeight) : minHeight;
      setHeight(height);
    }
  }
} // namespace imp