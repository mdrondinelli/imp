#include "ColumnWidget.h"

#include <gsl/gsl>

#include <algorithm>

namespace imp {
  void ColumnWidget::add(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(widget);
    Expects(flexibility >= 0.0f);
    widgets_.emplace_back(widget, flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void ColumnWidget::add(
      std::shared_ptr<Widget> widget, float flexibility, std::size_t i) {
    Expects(widget);
    Expects(flexibility >= 0.0f);
    Expects(i <= widgets_.size());
    widgets_.emplace(widgets_.begin() + i, std::move(widget), flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  bool ColumnWidget::remove(std::shared_ptr<Widget> widget) {
    auto it =
        std::find_if(widgets_.begin(), widgets_.end(), [&](auto const &pair) {
          return pair.first == widget;
        });
    if (it != widgets_.end()) {
      if (it->second) {
        --flexibleWidgetCount_;
      }
      widgets_.erase(it);
      return true;
    } else {
      return false;
    }
  }

  unsigned ColumnWidget::getMinWidth() const {
    auto minWidth = 0u;
    for (auto const &[widget, _] : widgets_) {
      minWidth = std::max(minWidth, widget->getMinWidth());
    }
    return minWidth;
  }

  unsigned ColumnWidget::getMinHeight() const {
    auto flexibilityFactor = 0.0f;
    auto flexibilitySum = 0.0f;
    auto minHeight = 0u;
    for (auto const &[widget, flexibility] : widgets_) {
      if (flexibility) {
        flexibilityFactor =
            std::max(flexibilityFactor, widget->getMinHeight() / flexibility);
        flexibilitySum += flexibility;
      } else {
        minHeight += widget->getMinHeight();
      }
    }
    minHeight +=
        static_cast<unsigned>(std::round(flexibilityFactor * flexibilitySum));
    return minHeight;
  }

  void ColumnWidget::layout() {
    if (flexibleWidgetCount_) {
      auto flexibilityFactor = 0.0f;
      auto flexibilitySum = 0.0f;
      auto rigidHeight = 0u;
      for (auto const &[widget, flexibility] : widgets_) {
        if (flexibility) {
          flexibilityFactor =
              std::max(flexibilityFactor, widget->getMinHeight() / flexibility);
          flexibilitySum += flexibility;
        } else {
          rigidHeight += widget->getMinHeight();
        }
      }
      if (auto preferredTotalHeight = getPreferredHeight()) {
        auto minFlexibleHeight = static_cast<unsigned>(
            std::round(flexibilityFactor * flexibilitySum));
        auto minTotalHeight = rigidHeight + minFlexibleHeight;
        auto totalHeight = std::max(*preferredTotalHeight, minTotalHeight);
        auto flexibleHeight = totalHeight - rigidHeight;
        flexibilityFactor = flexibleHeight / flexibilitySum;
      }
      auto preferredWidth = getPreferredWidth();
      auto width = preferredWidth ? std::max(*preferredWidth, getMinWidth())
                                  : getMinWidth();
      auto translation = 0.0f;
      for (auto const &[widget, flexibility] : widgets_) {
        if (flexibility) {
          auto minY = static_cast<unsigned>(std::round(translation));
          translation += flexibilityFactor * flexibility;
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
      }
      setWidth(width);
      setHeight(static_cast<unsigned>(std::round(translation)));
    } else {
      auto preferredWidth = getPreferredWidth();
      auto width = preferredWidth ? std::max(*preferredWidth, getMinWidth())
                                  : getMinWidth();
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
      setWidth(width);
      setHeight(height);
    }
  }
} // namespace imp