#include "gtest/gtest.h"

#include <memory>

#include <ui/BoxWidget.h>
#include <ui/RowWidget.h>

TEST(RowWidgetTest, handlesRigidMinExtent) {
  auto row = imp::RowWidget{};
  row.add(std::make_shared<imp::BoxWidget>(80u, 20u), 0.0f);
  row.add(std::make_shared<imp::BoxWidget>(20u, 80u), 0.0f);
  row.add(std::make_shared<imp::BoxWidget>(40u, 40u), 0.0f);
  EXPECT_EQ(row.getMinWidth(), 140u);
  EXPECT_EQ(row.getMinHeight(), 80u);
}

TEST(RowWidgetTest, handlesFlexibleMinExtent) {
  auto row = imp::RowWidget{};
  row.add(std::make_shared<imp::BoxWidget>(80u, 20u), 1.0f);
  row.add(std::make_shared<imp::BoxWidget>(20u, 80u), 0.0f);
  row.add(std::make_shared<imp::BoxWidget>(40u, 40u), 2.0f);
  EXPECT_EQ(row.getMinWidth(), 260u);
  EXPECT_EQ(row.getMinHeight(), 80u);
}

TEST(RowWidgetTest, handlesRigid) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.add(one, 0.0f);
  row.add(two, 0.0f);
  row.add(three, 0.0f);
  row.layout();
  EXPECT_EQ(row.getWidth(), 140u);
  EXPECT_EQ(row.getHeight(), 80u);
  EXPECT_EQ(one->getTranslation(), Eigen::Vector2i(0, 0));
  EXPECT_EQ(two->getTranslation(), Eigen::Vector2i(80, 0));
  EXPECT_EQ(three->getTranslation(), Eigen::Vector2i(100, 0));
  row.setPreferredWidth(20u);
  row.setPreferredHeight(20u);
  row.layout();
  EXPECT_EQ(row.getWidth(), 140u);
  EXPECT_EQ(row.getHeight(), 80u);
  EXPECT_EQ(one->getTranslation(), Eigen::Vector2i(0, 0));
  EXPECT_EQ(two->getTranslation(), Eigen::Vector2i(80, 0));
  EXPECT_EQ(three->getTranslation(), Eigen::Vector2i(100, 0));
  row.setPreferredWidth(200u);
  row.setPreferredHeight(200u);
  row.layout();
  EXPECT_EQ(row.getWidth(), 200u);
  EXPECT_EQ(row.getHeight(), 200u);
  EXPECT_EQ(one->getTranslation(), Eigen::Vector2i(0, 0));
  EXPECT_EQ(two->getTranslation(), Eigen::Vector2i(80, 0));
  EXPECT_EQ(three->getTranslation(), Eigen::Vector2i(100, 0));
}

TEST(BoxWidgetTest, handlesFlexible) {
  auto one = std::make_shared<imp::BoxWidget>(80u, 20u);
  auto two = std::make_shared<imp::BoxWidget>(20u, 80u);
  auto three = std::make_shared<imp::BoxWidget>(40u, 40u);
  auto row = imp::RowWidget{};
  row.add(one, 1.0f);
  row.add(two, 0.0f);
  row.add(three, 2.0f);
  row.layout();
  EXPECT_EQ(row.getWidth(), 260u);
  EXPECT_EQ(row.getHeight(), 80u);
  EXPECT_EQ(*one->getPreferredWidth(), 80u);
  EXPECT_EQ(*one->getPreferredHeight(), 80u);
  EXPECT_EQ(one->getTranslation(), Eigen::Vector2i(0, 0));
  EXPECT_EQ(two->getPreferredWidth(), std::nullopt);
  EXPECT_EQ(*two->getPreferredHeight(), 80u);
  EXPECT_EQ(two->getTranslation(), Eigen::Vector2i(80, 0));
  EXPECT_EQ(*three->getPreferredWidth(), 160u);
  EXPECT_EQ(*three->getPreferredHeight(), 80u);
  EXPECT_EQ(three->getTranslation(), Eigen::Vector2i(100, 0));
  row.setPreferredWidth(20u);
  row.setPreferredHeight(20u);
  row.layout();
  EXPECT_EQ(row.getWidth(), 260u);
  EXPECT_EQ(row.getHeight(), 80u);
  EXPECT_EQ(*one->getPreferredWidth(), 80u);
  EXPECT_EQ(*one->getPreferredHeight(), 80u);
  EXPECT_EQ(one->getTranslation(), Eigen::Vector2i(0, 0));
  EXPECT_EQ(two->getPreferredWidth(), std::nullopt);
  EXPECT_EQ(*two->getPreferredHeight(), 80u);
  EXPECT_EQ(two->getTranslation(), Eigen::Vector2i(80, 0));
  EXPECT_EQ(*three->getPreferredWidth(), 160u);
  EXPECT_EQ(*three->getPreferredHeight(), 80u);
  EXPECT_EQ(three->getTranslation(), Eigen::Vector2i(100, 0));
  row.setPreferredWidth(320u);
  row.setPreferredHeight(160u);
  row.layout();
  EXPECT_EQ(row.getWidth(), 320u);
  EXPECT_EQ(row.getHeight(), 160u);
  EXPECT_EQ(*one->getPreferredWidth(), 100u);
  EXPECT_EQ(*one->getPreferredHeight(), 160u);
  EXPECT_EQ(one->getTranslation(), Eigen::Vector2i(0, 0));
  EXPECT_EQ(two->getPreferredWidth(), std::nullopt);
  EXPECT_EQ(*two->getPreferredHeight(), 160u);
  EXPECT_EQ(two->getTranslation(), Eigen::Vector2i(100, 0));
  EXPECT_EQ(*three->getPreferredWidth(), 200u);
  EXPECT_EQ(*three->getPreferredHeight(), 160u);
  EXPECT_EQ(three->getTranslation(), Eigen::Vector2i(120, 0));
}