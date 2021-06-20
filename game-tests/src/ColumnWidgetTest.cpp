#include "gtest/gtest.h"

#include <memory>

#include <ui/BoxWidget.h>
#include <ui/ColumnWidget.h>

TEST(ColumnWidgetTest, handlesRigidMinExtent) {
  auto row = imp::ColumnWidget{};
  row.pushBack(std::make_shared<imp::BoxWidget>(80u, 20u), 0.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(20u, 80u), 0.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(40u, 40u), 0.0f);
  EXPECT_EQ(row.getMinWidth(), 80);
  EXPECT_EQ(row.getMinHeight(), 140);
}

TEST(ColumnWidgetTest, handlesFlexibleMinExtent) {
  auto row = imp::ColumnWidget{};
  row.pushBack(std::make_shared<imp::BoxWidget>(80u, 20u), 1.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(20u, 80u), 0.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(40u, 40u), 2.0f);
  EXPECT_EQ(row.getMinWidth(), 80);
  EXPECT_EQ(row.getMinHeight(), 140);
}

TEST(ColumnWidgetTest, handlesRigidNoPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto col = imp::ColumnWidget{};
  col.pushBack(one, 0.0f);
  col.pushBack(two, 0.0f);
  col.pushBack(three, 0.0f);
  col.layout();
  // col
  EXPECT_EQ(col.getWidth(), 80);
  EXPECT_EQ(col.getHeight(), 140);
  // one
  EXPECT_EQ(one->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(one->getPreferredHeight(), 0);
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(two->getPreferredHeight(), 0);
  EXPECT_EQ(two->getTranslation()(0), 0);
  EXPECT_EQ(two->getTranslation()(1), 20);
  // three
  EXPECT_EQ(three->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(three->getPreferredHeight(), 0);
  EXPECT_EQ(three->getTranslation()(0), 0);
  EXPECT_EQ(three->getTranslation()(1), 100);
}

TEST(ColumnWidgetTest, handlesRigidSmallPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto col = imp::ColumnWidget{};
  col.pushBack(one, 0.0f);
  col.pushBack(two, 0.0f);
  col.pushBack(three, 0.0f);
  col.setPreferredWidth(20u);
  col.setPreferredHeight(20u);
  col.layout();
  // col
  EXPECT_EQ(col.getWidth(), 80);
  EXPECT_EQ(col.getHeight(), 140);
  // one
  EXPECT_EQ(one->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(one->getPreferredHeight(), 0);
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(two->getPreferredHeight(), 0);
  EXPECT_EQ(two->getTranslation()(0), 0);
  EXPECT_EQ(two->getTranslation()(1), 20);
  // three
  EXPECT_EQ(three->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(three->getPreferredHeight(), 0);
  EXPECT_EQ(three->getTranslation()(0), 0);
  EXPECT_EQ(three->getTranslation()(1), 100);
}

TEST(ColumnWidgetTest, handlesRigidLargePreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto col = imp::ColumnWidget{};
  col.pushBack(one, 0.0f);
  col.pushBack(two, 0.0f);
  col.pushBack(three, 0.0f);
  col.setPreferredWidth(200u);
  col.setPreferredHeight(200u);
  col.layout();
  // col
  EXPECT_EQ(col.getWidth(), col.getPreferredWidth());
  EXPECT_EQ(col.getHeight(), col.getPreferredHeight());
  // one
  EXPECT_EQ(one->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(one->getPreferredHeight(), 0);
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(two->getPreferredHeight(), 0);
  EXPECT_EQ(two->getTranslation()(0), 0);
  EXPECT_EQ(two->getTranslation()(1), 20);
  // three
  EXPECT_EQ(three->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(three->getPreferredHeight(), 0);
  EXPECT_EQ(three->getTranslation()(0), 0);
  EXPECT_EQ(three->getTranslation()(1), 100);
}

TEST(ColumnWidgetTest, handlesFlexibleNoPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto col = imp::ColumnWidget{};
  col.pushBack(one, 1.0f);
  col.pushBack(two, 0.0f);
  col.pushBack(three, 2.0f);
  col.layout();
  // col
  EXPECT_EQ(col.getWidth(), 80);
  EXPECT_EQ(col.getHeight(), 140);
  // one
  EXPECT_EQ(one->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(one->getPreferredHeight(), 20);
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(two->getPreferredHeight(), 0);
  EXPECT_EQ(two->getTranslation()(0), 0);
  EXPECT_EQ(two->getTranslation()(1), 20);
  // three
  EXPECT_EQ(three->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(three->getPreferredHeight(), 40);
  EXPECT_EQ(three->getTranslation()(0), 0);
  EXPECT_EQ(three->getTranslation()(1), 100);
}

TEST(ColumnWidgetTest, handlesFlexibleSmallPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto col = imp::ColumnWidget{};
  col.pushBack(one, 1.0f);
  col.pushBack(two, 0.0f);
  col.pushBack(three, 2.0f);
  col.setPreferredWidth(20u);
  col.setPreferredHeight(20u);
  col.layout();
  // col
  EXPECT_EQ(col.getWidth(), 80);
  EXPECT_EQ(col.getHeight(), 140);
  // one
  EXPECT_EQ(one->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(one->getPreferredHeight(), 20);
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(two->getPreferredHeight(), 0);
  EXPECT_EQ(two->getTranslation()(0), 0);
  EXPECT_EQ(two->getTranslation()(1), 20);
  // three
  EXPECT_EQ(three->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(three->getPreferredHeight(), 40);
  EXPECT_EQ(three->getTranslation()(0), 0);
  EXPECT_EQ(three->getTranslation()(1), 100);
}

TEST(ColumnWidgetTest, handlesFlexibleLargePreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto col = imp::ColumnWidget{};
  col.pushBack(one, 1.0f);
  col.pushBack(two, 0.0f);
  col.pushBack(three, 2.0f);
  col.setPreferredWidth(320u);
  col.setPreferredHeight(200u);
  col.layout();
  // col
  EXPECT_EQ(col.getWidth(), col.getPreferredWidth());
  EXPECT_EQ(col.getHeight(), col.getPreferredHeight());
  // one
  EXPECT_EQ(one->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(one->getPreferredHeight(), 40);
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(two->getPreferredHeight(), 0);
  EXPECT_EQ(two->getTranslation()(0), 0);
  EXPECT_EQ(two->getTranslation()(1), 40);
  // three
  EXPECT_EQ(three->getPreferredWidth(), col.getWidth());
  EXPECT_EQ(three->getPreferredHeight(), 80);
  EXPECT_EQ(three->getTranslation()(0), 0);
  EXPECT_EQ(three->getTranslation()(1), 120);
}