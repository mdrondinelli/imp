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
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void RowWidget::pushBack(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(flexibility >= 0.0f);
    widgets_.emplace_back(std::move(widget), flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void RowWidget::insert(
      std::size_t index, std::shared_ptr<Widget> widget, float flexibility) {
    Expects(index <= widgets_.size());
    Expects(flexibility >= 0.0f);
    widgets_.emplace(widgets_.begin() + index, std::move(widget), flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void RowWidget::popFront() {
    Expects(!widgets_.empty());
    if (widgets_.front().second) {
      --flexibleWidgetCount_;
    }
    widgets_.erase(widgets_.begin());
  }

  void RowWidget::popBack() {
    Expects(!widgets_.empty());
    if (widgets_.back().second) {
      --flexibleWidgetCount_;
    }
    widgets_.pop_back();
  }

  void RowWidget::erase(std::size_t index) {
    Expects(index <= widgets_.size());
    if (widgets_[index].second) {
      --flexibleWidgetCount_;
    }
    widgets_.erase(widgets_.begin() + index);
  }

  unsigned RowWidget::getMinWidth() const {
    auto flexibilityScale = 0.0f;
    auto flexibilitySum = 0.0f;
    auto rigidWidth = 0u;
    for (auto const &[widget, flexibility] : widgets_) {
      if (widget) {
        if (flexibility) {
          flexibilityScale =
              std::max(flexibilityScale, widget->getMinWidth() / flexibility);
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
    auto preferredHeight = getPreferredHeight();
    auto height = preferredHeight ? std::max(*preferredHeight, getMinHeight())
                                  : getMinHeight();
    setHeight(height);
    if (flexibleWidgetCount_) {
      auto flexibilityScale = 0.0f;
      auto flexibilitySum = 0.0f;
      auto rigidWidth = 0u;
      for (auto const &[widget, flexibility] : widgets_) {
        if (widget) {
          if (flexibility) {
            flexibilityScale =
                std::max(flexibilityScale, widget->getMinWidth() / flexibility);
          } else {
            rigidWidth += widget->getMinWidth();
          }
        }
        flexibilitySum += flexibility;
      }
      if (auto preferredTotalWidth = getPreferredWidth()) {
        auto minFlexibleWidth = static_cast<unsigned>(
            std::round(flexibilityScale * flexibilitySum));
        auto minTotalWidth = rigidWidth + minFlexibleWidth;
        auto totalWidth = std::max(*preferredTotalWidth, minTotalWidth);
        auto flexibleWidth = totalWidth - rigidWidth;
        flexibilityScale = flexibleWidth / flexibilitySum;
      }
      auto translation = 0.0f;
      for (auto const &[widget, flexibility] : widgets_) {
        if (widget) {
          if (flexibility) {
            auto minX = static_cast<unsigned>(std::round(translation));
            translation += flexibilityScale * flexibility;
            auto maxX = static_cast<unsigned>(std::round(translation));
            widget->setPreferredWidth(maxX - minX);
            widget->setPreferredHeight(height);
            widget->setTranslation({minX, 0});
            widget->layout();
          } else {
            widget->setPreferredWidth(std::nullopt);
            widget->setPreferredHeight(height);
            widget->setTranslation({std::round(translation), 0});
            widget->layout();
            translation += widget->getWidth();
          }
        } else {
          translation += flexibilityScale * flexibility;
        }
      }
      setWidth(static_cast<unsigned>(std::round(translation)));
    } else {
      auto minWidth = 0u;
      for (auto const &[widget, _] : widgets_) {
        if (widget) {
          widget->setPreferredWidth(std::nullopt);
          widget->setPreferredHeight(height);
          widget->setTranslation({minWidth, 0});
          widget->layout();
          minWidth += widget->getWidth();
        }
      }
      auto preferredWidth = getPreferredWidth();
      auto width =
          preferredWidth ? std::max(*preferredWidth, minWidth) : minWidth;
      setWidth(width);
    }
  }
} // namespace imp