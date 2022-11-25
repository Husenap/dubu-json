#include <sstream>

#include <dubu_json/dubu_json.hpp>
#include <gtest/gtest.h>

TEST(dubu_json, empty_object) {
  const auto json = dubu::json::parse("{}");
  EXPECT_NO_THROW(json.asObject());
}
