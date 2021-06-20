#include "gtest/gtest.h"

#include <memory>

#include <ui/BoxWidget.h>
#include <ui/RowWidget.h>

TEST(RowWidgetTest, handlesRigidMinExtent) {
  auto row = imp::RowWidget{};
  row.pushBack(std::make_shared<imp::BoxWidget>(80u, 20u), 0.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(20u, 80u), 0.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(40u, 40u), 0.0f);
  EXPECT_EQ(row.getMinWidth(), 140);
  EXPECT_EQ(row.getMinHeight(), 80);
}

TEST(RowWidgetTest, handlesFlexibleMinExtent) {
  auto row = imp::RowWidget{};
  row.pushBack(std::make_shared<imp::BoxWidget>(80u, 20u), 1.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(20u, 80u), 0.0f);
  row.pushBack(std::make_shared<imp::BoxWidget>(40u, 40u), 2.0f);
  EXPECT_EQ(row.getMinWidth(), 260);
  EXPECT_EQ(row.getMinHeight(), 80);
}

TEST(RowWidgetTest, handlesRigidNoPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.pushBack(one, 0.0f);
  row.pushBack(two, 0.0f);
  row.pushBack(three, 0.0f);
  row.layout();
  // row
  EXPECT_EQ(row.getWidth(), 140);
  EXPECT_EQ(row.getHeight(), 80);
  // one
  EXPECT_EQ(one->getPreferredWidth(), 0);
  EXPECT_EQ(one->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), 0);
  EXPECT_EQ(two->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(two->getTranslation()(0), 80);
  EXPECT_EQ(two->getTranslation()(1), 0);
  // three
  EXPECT_EQ(three->getPreferredWidth(), 0);
  EXPECT_EQ(three->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(three->getTranslation()(0), 100);
  EXPECT_EQ(three->getTranslation()(1), 0);
}

TEST(RowWidgetTest, handlesRigidSmallPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.pushBack(one, 0.0f);
  row.pushBack(two, 0.0f);
  row.pushBack(three, 0.0f);
  row.setPreferredWidth(20u);
  row.setPreferredHeight(20u);
  row.layout();
  // row
  EXPECT_EQ(row.getWidth(), 140);
  EXPECT_EQ(row.getHeight(), 80);
  // one
  EXPECT_EQ(one->getPreferredWidth(), 0);
  EXPECT_EQ(one->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), 0);
  EXPECT_EQ(two->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(two->getTranslation()(0), 80);
  EXPECT_EQ(two->getTranslation()(1), 0);
  // three
  EXPECT_EQ(three->getPreferredWidth(), 0);
  EXPECT_EQ(three->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(three->getTranslation()(0), 100);
  EXPECT_EQ(three->getTranslation()(1), 0);
}

TEST(RowWidgetTest, handlesRigidLargePreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.pushBack(one, 0.0f);
  row.pushBack(two, 0.0f);
  row.pushBack(three, 0.0f);
  row.setPreferredWidth(200u);
  row.setPreferredHeight(200u);
  row.layout();
  // row
  EXPECT_EQ(row.getWidth(), row.getPreferredWidth());
  EXPECT_EQ(row.getHeight(), row.getPreferredHeight());
  // one
  EXPECT_EQ(one->getPreferredWidth(), 0);
  EXPECT_EQ(one->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), 0);
  EXPECT_EQ(two->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(two->getTranslation()(0), 80);
  EXPECT_EQ(two->getTranslation()(1), 0);
  // three
  EXPECT_EQ(three->getPreferredWidth(), 0);
  EXPECT_EQ(three->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(three->getTranslation()(0), 100);
  EXPECT_EQ(three->getTranslation()(1), 0);
}

TEST(RowWidgetTest, handlesFlexibleNoPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.pushBack(one, 1.0f);
  row.pushBack(two, 0.0f);
  row.pushBack(three, 2.0f);
  row.layout();
  // row
  EXPECT_EQ(row.getWidth(), 260);
  EXPECT_EQ(row.getHeight(), 80);
  // one
  EXPECT_EQ(one->getPreferredWidth(), 80);
  EXPECT_EQ(one->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), 0);
  EXPECT_EQ(two->getPreferredHeight(), 80);
  EXPECT_EQ(two->getTranslation()(0), 80);
  EXPECT_EQ(two->getTranslation()(1), 0);
  // three
  EXPECT_EQ(three->getPreferredWidth(), 160);
  EXPECT_EQ(three->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(three->getTranslation()(0), 100);
  EXPECT_EQ(three->getTranslation()(1), 0);
}

TEST(RowWidgetTest, handlesFlexibleSmallPreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.pushBack(one, 1.0f);
  row.pushBack(two, 0.0f);
  row.pushBack(three, 2.0f);
  row.setPreferredWidth(20u);
  row.setPreferredHeight(20u);
  row.layout();
  // row
  EXPECT_EQ(row.getWidth(), 260);
  EXPECT_EQ(row.getHeight(), 80);
  // one
  EXPECT_EQ(one->getPreferredWidth(), 80);
  EXPECT_EQ(one->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), 0);
  EXPECT_EQ(two->getPreferredHeight(), 80);
  EXPECT_EQ(two->getTranslation()(0), 80);
  EXPECT_EQ(two->getTranslation()(1), 0);
  // three
  EXPECT_EQ(three->getPreferredWidth(), 160);
  EXPECT_EQ(three->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(three->getTranslation()(0), 100);
  EXPECT_EQ(three->getTranslation()(1), 0);
}

TEST(RowWidgetTest, handlesFlexibleLargePreference) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.pushBack(one, 1.0f);
  row.pushBack(two, 0.0f);
  row.pushBack(three, 2.0f);
  row.setPreferredWidth(320u);
  row.setPreferredHeight(160u);
  row.layout();
  // row
  EXPECT_EQ(row.getWidth(), row.getPreferredWidth());
  EXPECT_EQ(row.getHeight(), row.getPreferredHeight());
  // one
  EXPECT_EQ(one->getPreferredWidth(), 100);
  EXPECT_EQ(one->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(one->getTranslation()(0), 0);
  EXPECT_EQ(one->getTranslation()(1), 0);
  // two
  EXPECT_EQ(two->getPreferredWidth(), 0);
  EXPECT_EQ(two->getPreferredHeight(), row.getHeight());
  EXPECT_EQ(two->getTranslation()(0), 100);
  EXPECT_EQ(two->getTranslation()(1), 0);
  // three
  EXPECT_EQ(three->getPreferredWidth(), 200);
  EXPECT_EQ(three->getPreferredHeight(), 160u);
  EXPECT_EQ(three->getTranslation()(0), 120);
  EXPECT_EQ(three->getTranslation()(1), 0);
}