#include "gtest/gtest.h"

#include <ui/BoxWidget.h>

TEST(BoxWidgetTest, handlesNoPreference) {
  auto widget = imp::BoxWidget{80u, 40u};
  widget.layout();
  EXPECT_EQ(widget.getWidth(), 80u);
  EXPECT_EQ(widget.getHeight(), 40u);
}

TEST(BoxWidgetTest, handlesSmallPreference) {
  auto widget = imp::BoxWidget{80u, 40u};
  widget.setPreferredWidth(60u);
  widget.setPreferredHeight(20u);
  widget.layout();
  EXPECT_EQ(widget.getWidth(), 80u);
  EXPECT_EQ(widget.getHeight(), 40u);
}

TEST(BoxWidgetTest, handlesLargePreference) {
  auto widget = imp::BoxWidget{80u, 40u};
  widget.setPreferredWidth(100u);
  widget.setPreferredHeight(60u);
  widget.layout();
  EXPECT_EQ(widget.getWidth(), 100u);
  EXPECT_EQ(widget.getHeight(), 60u);
}