#include <sstream>

#include <dubu_json/dubu_json.hpp>
#include <gtest/gtest.h>

TEST(dubu_json_type, undefined) {
  dubu::json::Value value;
  EXPECT_TRUE(value.isNull());
  EXPECT_THROW(value.asObject(), std::bad_any_cast);
  EXPECT_THROW(value.asArray(), std::bad_any_cast);
  EXPECT_THROW(value.asString(), std::bad_any_cast);
  EXPECT_THROW(value.asNumber(), std::bad_any_cast);
  EXPECT_THROW(value.asBoolean(), std::bad_any_cast);
}

TEST(dubu_json_type, defined) {
  dubu::json::Value value;
  EXPECT_TRUE(value.isNull());

  value = 3.1415;
  EXPECT_FALSE(value.isNull());
  EXPECT_NO_THROW(value.asNumber());
  EXPECT_DOUBLE_EQ(value.asNumber(), 3.1415);

  value = dubu::json::Object{};
  EXPECT_NO_THROW(value.asObject());
  EXPECT_EQ(value.asObject().size(), 0);

  value = dubu::json::Array{};
  EXPECT_NO_THROW(value.asArray());
  EXPECT_EQ(value.asArray().size(), 0);

  static constexpr auto Text = "hello json";

  value = std::string(Text);
  EXPECT_NO_THROW(value.asString());
  EXPECT_EQ(value.asString(), Text);
  value = std::string_view(Text);
  EXPECT_NO_THROW(value.asString());
  EXPECT_EQ(value.asString(), Text);
  value = Text;
  EXPECT_NO_THROW(value.asString());
  EXPECT_EQ(value.asString(), Text);

  value = true;
  EXPECT_NO_THROW(value.asBoolean());
  EXPECT_TRUE(value.asBoolean());
  value = false;
  EXPECT_NO_THROW(value.asBoolean());
  EXPECT_FALSE(value.asBoolean());
}