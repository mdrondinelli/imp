#pragma once

#include <memory>
#include <vector>

#include <gsl/gsl>

#include "Widget.h"

namespace imp {
  class RowWidget: public Widget {
  public:
    gsl::span<std::pair<std::shared_ptr<Widget>, float> const>
    getWidgets() const noexcept;

    void pushFront(std::shared_ptr<Widget> widget, float flexibility);

    void pushBack(std::shared_ptr<Widget> widget, float flexibility);

    void insert(
        std::size_t index, std::shared_ptr<Widget> widget, float flexibility);

    void popFront();

    void popBack();

    void erase(std::size_t index);

    unsigned getMinWidth() const override;

    unsigned getMinHeight() const override;

    void layout() override;

  private:
    std::vector<std::pair<std::shared_ptr<Widget>, float>> widgets_;
  };
} // namespace imp