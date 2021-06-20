#include "gtest/gtest.h"

#include <ui/BoxWidget.h>
#include <ui/ContainerWidget.h>

TEST(ContainerWidgetTest, handlesMinima) {
  auto box = std::make_shared<imp::BoxWidget>(32u, 16u);
  auto container = imp::ContainerWidget{box};
  EXPECT_EQ(container.getMinWidth(), 32u);
  EXPECT_EQ(container.getMinHeight(), 16u);
  container.setMinWidth(40u);
  container.setMinHeight(24u);
  EXPECT_EQ(container.getMinWidth(), 40u);
  EXPECT_EQ(container.getMinHeight(), 24u);
}

TEST(ContainerWidgetTest, handlesNoPreference) {
  auto box = std::make_shared<imp::BoxWidget>(32u, 16u);
  auto container = imp::ContainerWidget{box};
  container.setHorizontalAlignment(imp::Alignment::NEGATIVE);
  container.setVerticalAlignment(imp::Alignment::NEGATIVE);
  container.layout();
  EXPECT_EQ(container.getWidth(), box->getWidth());
  EXPECT_EQ(container.getHeight(), box->getHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 0);
  EXPECT_EQ(box->getTranslation()(1), 0);
  container.setHorizontalAlignment(imp::Alignment::CENTER);
  container.setVerticalAlignment(imp::Alignment::CENTER);
  container.layout();
  EXPECT_EQ(container.getWidth(), box->getWidth());
  EXPECT_EQ(container.getHeight(), box->getHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 0);
  EXPECT_EQ(box->getTranslation()(1), 0);
  container.setHorizontalAlignment(imp::Alignment::POSITIVE);
  container.setVerticalAlignment(imp::Alignment::POSITIVE);
  container.layout();
  EXPECT_EQ(container.getWidth(), box->getWidth());
  EXPECT_EQ(container.getHeight(), box->getHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 0);
  EXPECT_EQ(box->getTranslation()(1), 0);
}

TEST(ContainerWidgetTest, handlesSmallPreference) {
  auto box = std::make_shared<imp::BoxWidget>(32u, 16u);
  auto container = imp::ContainerWidget{box};
  container.setPreferredWidth(24u);
  container.setPreferredHeight(8u);
  container.setHorizontalAlignment(imp::Alignment::NEGATIVE);
  container.setVerticalAlignment(imp::Alignment::NEGATIVE);
  container.layout();
  EXPECT_EQ(container.getWidth(), box->getWidth());
  EXPECT_EQ(container.getHeight(), box->getHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 0);
  EXPECT_EQ(box->getTranslation()(1), 0);
  container.setHorizontalAlignment(imp::Alignment::CENTER);
  container.setVerticalAlignment(imp::Alignment::CENTER);
  container.layout();
  EXPECT_EQ(container.getWidth(), box->getWidth());
  EXPECT_EQ(container.getHeight(), box->getHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 0);
  EXPECT_EQ(box->getTranslation()(1), 0);
  container.setHorizontalAlignment(imp::Alignment::POSITIVE);
  container.setVerticalAlignment(imp::Alignment::POSITIVE);
  container.layout();
  EXPECT_EQ(container.getWidth(), box->getWidth());
  EXPECT_EQ(container.getHeight(), box->getHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 0);
  EXPECT_EQ(box->getTranslation()(1), 0);
}

TEST(ContainerWidgetTest, handlesLargePreference) {
  auto box = std::make_shared<imp::BoxWidget>(32u, 16u);
  auto container = imp::ContainerWidget{box};
  container.setPreferredWidth(40u);
  container.setPreferredHeight(24u);
  container.setHorizontalAlignment(imp::Alignment::NEGATIVE);
  container.setVerticalAlignment(imp::Alignment::NEGATIVE);
  container.layout();
  EXPECT_EQ(container.getWidth(), *container.getPreferredWidth());
  EXPECT_EQ(container.getHeight(), *container.getPreferredHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 0);
  EXPECT_EQ(box->getTranslation()(1), 0);
  container.setHorizontalAlignment(imp::Alignment::CENTER);
  container.setVerticalAlignment(imp::Alignment::CENTER);
  container.layout();
  EXPECT_EQ(container.getWidth(), *container.getPreferredWidth());
  EXPECT_EQ(container.getHeight(), *container.getPreferredHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 4);
  EXPECT_EQ(box->getTranslation()(1), 4);
  container.setHorizontalAlignment(imp::Alignment::POSITIVE);
  container.setVerticalAlignment(imp::Alignment::POSITIVE);
  container.layout();
  EXPECT_EQ(container.getWidth(), *container.getPreferredWidth());
  EXPECT_EQ(container.getHeight(), *container.getPreferredHeight());
  EXPECT_FALSE(box->getPreferredWidth().has_value());
  EXPECT_FALSE(box->getPreferredHeight().has_value());
  EXPECT_EQ(box->getTranslation()(0), 8);
  EXPECT_EQ(box->getTranslation()(1), 8);
}