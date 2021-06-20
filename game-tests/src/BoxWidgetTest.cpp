#include "gtest/gtest.h"

#include <ui/BoxWidget.h>

TEST(BoxWidgetTest, handlesNoPreference) {
  auto widget = imp::BoxWidget{80u, 40u};
  widget.layout();
  EXPECT_EQ(widget.getWidth(), 80);
  EXPECT_EQ(widget.getHeight(), 40);
}

TEST(BoxWidgetTest, handlesSmallPreference) {
  auto widget = imp::BoxWidget{80u, 40u};
  widget.setPreferredWidth(60u);
  widget.setPreferredHeight(20u);
  widget.layout();
  EXPECT_EQ(widget.getWidth(), 80);
  EXPECT_EQ(widget.getHeight(), 40);
}

TEST(BoxWidgetTest, handlesLargePreference) {
  auto widget = imp::BoxWidget{80u, 40u};
  widget.setPreferredWidth(100u);
  widget.setPreferredHeight(60u);
  widget.layout();
  EXPECT_EQ(widget.getWidth(), 100);
  EXPECT_EQ(widget.getHeight(), 60);
}