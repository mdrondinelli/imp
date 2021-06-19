#include "RowWidget.h"

#include <gsl/gsl>

#include <algorithm>

namespace imp {
  void RowWidget::add(std::shared_ptr<Widget> widget, float flexibility) {
    Expects(widget);
    Expects(flexibility >= 0.0f);
    widgets_.emplace_back(widget, flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  void RowWidget::add(
      std::shared_ptr<Widget> widget, float flexibility, std::size_t i) {
    Expects(widget);
    Expects(flexibility >= 0.0f);
    Expects(i <= widgets_.size());
    widgets_.emplace(widgets_.begin() + i, std::move(widget), flexibility);
    if (flexibility) {
      ++flexibleWidgetCount_;
    }
  }

  bool RowWidget::remove(std::shared_ptr<Widget> widget) {
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

  unsigned RowWidget::getMinWidth() const {
    auto flexibilityFactor = 0.0f;
    auto flexibilitySum = 0.0f;
    auto minWidth = 0u;
    for (auto const &[widget, flexibility] : widgets_) {
      if (flexibility) {
        flexibilityFactor =
            std::max(flexibilityFactor, widget->getMinWidth() / flexibility);
        flexibilitySum += flexibility;
      } else {
        minWidth += widget->getMinWidth();
      }
    }
    minWidth +=
        static_cast<unsigned>(std::round(flexibilityFactor * flexibilitySum));
    return minWidth;
  }

  unsigned RowWidget::getMinHeight() const {
    auto minHeight = 0u;
    for (auto const &[widget, _] : widgets_) {
      minHeight = std::max(minHeight, widget->getMinHeight());
    }
    return minHeight;
  }

  void RowWidget::layout() {
    if (flexibleWidgetCount_) {
      auto flexibilityFactor = 0.0f;
      auto flexibilitySum = 0.0f;
      auto rigidWidth = 0u;
      for (auto const &[widget, flexibility] : widgets_) {
        if (flexibility) {
          flexibilityFactor =
              std::max(flexibilityFactor, widget->getMinWidth() / flexibility);
          flexibilitySum += flexibility;
        } else {
          rigidWidth += widget->getMinWidth();
        }
      }
      if (auto preferredTotalWidth = getPreferredWidth()) {
        auto minFlexibleWidth = static_cast<unsigned>(
            std::round(flexibilityFactor * flexibilitySum));
        auto minTotalWidth = rigidWidth + minFlexibleWidth;
        auto totalWidth = std::max(*preferredTotalWidth, minTotalWidth);
        auto flexibleWidth = totalWidth - rigidWidth;
        flexibilityFactor = flexibleWidth / flexibilitySum;
      }
      auto preferredHeight = getPreferredHeight();
      auto height = preferredHeight ? std::max(*preferredHeight, getMinHeight())
                                    : getMinHeight();
      auto translation = 0.0f;
      for (auto const &[widget, flexibility] : widgets_) {
        if (flexibility) {
          auto minX = static_cast<unsigned>(std::round(translation));
          translation += flexibilityFactor * flexibility;
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
      }
      setWidth(static_cast<unsigned>(std::round(translation)));
      setHeight(height);
    } else {
      auto preferredHeight = getPreferredHeight();
      auto height = preferredHeight ? std::max(*preferredHeight, getMinHeight())
                                    : getMinHeight();
      auto minWidth = 0u;
      for (auto const &[widget, _] : widgets_) {
        widget->setPreferredWidth(std::nullopt);
        widget->setPreferredHeight(height);
        widget->setTranslation({minWidth, 0});
        widget->layout();
        minWidth += widget->getWidth();
      }
      auto preferredWidth = getPreferredWidth();
      auto width =
          preferredHeight ? std::max(*preferredWidth, minWidth) : minWidth;
      setWidth(width);
      setHeight(height);
    }
  }
} // namespace imp