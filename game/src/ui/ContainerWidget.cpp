#include "ContainerWidget.h"

namespace imp {
  ContainerWidget::ContainerWidget(
      std::shared_ptr<Widget> content,
      Alignment horizontalAlignment,
      Alignment verticalAlignment,
      unsigned minWidth,
      unsigned minHeight) noexcept:
      content_{std::move(content)},
      horizontalAlignment_{horizontalAlignment},
      verticalAlignment_{verticalAlignment},
      minWidth_{minWidth},
      minHeight_{minHeight} {}

  void ContainerWidget::layout() {
    if (content_) {
      content_->setPreferredWidth(std::nullopt);
      content_->setPreferredHeight(std::nullopt);
      content_->layout();
      auto translation = Eigen::Vector2i{0, 0};
      auto contentWidth = content_->getWidth();
      auto preferredWidth = getPreferredWidth();
      auto width = preferredWidth
                       ? std::max({contentWidth, *preferredWidth, minWidth_})
                       : std::max(contentWidth, minWidth_);
      if (width > contentWidth) {
        switch (horizontalAlignment_) {
        case Alignment::NEGATIVE:
          translation(0) = 0;
          break;
        case Alignment::CENTER:
          translation(0) = (width - contentWidth) / 2;
          break;
        case Alignment::POSITIVE:
          translation(0) = width - contentWidth;
          break;
        }
      }
      auto contentHeight = content_->getHeight();
      auto preferredHeight = getPreferredHeight();
      auto height =
          preferredHeight
              ? std::max({contentHeight, *preferredHeight, minHeight_})
              : std::max(contentHeight, minHeight_);
      if (height > contentHeight) {
        switch (verticalAlignment_) {
        case Alignment::NEGATIVE:
          translation(1) = 0;
          break;
        case Alignment::CENTER:
          translation(1) = (height - contentHeight) / 2;
          break;
        case Alignment::POSITIVE:
          translation(1) = height - contentHeight;
          break;
        }
      }
      content_->setTranslation(translation);
      setWidth(width);
      setHeight(height);
    } else {
      setWidth(minWidth_);
      setHeight(minHeight_);
    }
  }

  std::shared_ptr<Widget> ContainerWidget::getContent() const noexcept {
    return content_;
  }

  void ContainerWidget::setContent(std::shared_ptr<Widget> content) noexcept {
    content_ = std::move(content);
  }

  Alignment ContainerWidget::getHorizontalAlignment() const noexcept {
    return horizontalAlignment_;
  }

  void ContainerWidget::setHorizontalAlignment(Alignment alignment) noexcept {
    horizontalAlignment_ = alignment;
  }

  Alignment ContainerWidget::getVerticalAligment() const noexcept {
    return verticalAlignment_;
  }

  void ContainerWidget::setVerticalAlignment(Alignment alignment) noexcept {
    verticalAlignment_ = alignment;
  }

  unsigned ContainerWidget::getMinWidth() const {
    return content_ ? std::max(content_->getMinWidth(), minWidth_) : minWidth_;
  }

  void ContainerWidget::setMinWidth(unsigned minWidth) noexcept {
    minWidth_ = minWidth;
  }

  unsigned ContainerWidget::getMinHeight() const {
    return content_ ? std::max(content_->getMinHeight(), minHeight_)
                    : minHeight_;
  }

  void ContainerWidget::setMinHeight(unsigned minHeight) noexcept {
    minHeight_ = minHeight;
  }
} // namespace imp