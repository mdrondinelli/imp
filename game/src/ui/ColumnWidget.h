#pragma once

#include <memory>
#include <vector>

#include "Widget.h"

namespace imp {
  class ColumnWidget: public Widget {
  public:
    void add(std::shared_ptr<Widget> widget, float flexibility);
    void add(std::shared_ptr<Widget> widget, float flexibility, std::size_t i);
    bool remove(std::shared_ptr<Widget> widget);

    unsigned getMinWidth() const override;
    unsigned getMinHeight() const override;
    void layout() override;

  private:
    std::vector<std::pair<std::shared_ptr<Widget>, float>> widgets_;
    std::size_t flexibleWidgetCount_{0};
  };
} // namespace imp