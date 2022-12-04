#include <sstream>

#include <dubu_json/dubu_json.hpp>
#include <gtest/gtest.h>

using namespace dubu;

TEST(dubu_json, empty_object) {
  const auto json = json::parse("{}");
  EXPECT_NO_THROW(json.asObject());
}

TEST(dubu_json, empty_array) {
  const auto json = json::parse("[]");
  EXPECT_NO_THROW(json.asArray());
}

TEST(dubu_json, array) {
  const auto  json = json::parse("[true, false, null, \"hello\", [], {}, 1337.42]");
  const auto& arr  = json.asArray();
  EXPECT_EQ(arr.size(), 7);
  EXPECT_TRUE(arr[0].asBoolean());
  EXPECT_FALSE(arr[1].asBoolean());
  EXPECT_TRUE(arr[2].isNull());
  EXPECT_EQ(arr[3].asString(), "hello");
  EXPECT_EQ(arr[4].asArray().size(), 0);
  EXPECT_EQ(arr[5].asObject().size(), 0);
  EXPECT_EQ(arr[6].asNumber(), 1337.42);
}

TEST(dubu_json, constants) {
  {
    const auto json = json::parse("true");
    EXPECT_TRUE(json.asBoolean());
  }
  {
    const auto json = json::parse("false");
    EXPECT_FALSE(json.asBoolean());
  }
  {
    const auto json = json::parse("null");
    EXPECT_TRUE(json.isNull());
  }
}

TEST(dubu_json, string) {
  {
    const auto json = json::parse("\"hello\\nworld\"");
    EXPECT_EQ(json.asString(), "hello\nworld");
  }
  {
    const auto json = json::parse("\"\\uc548\\ub155\\ud558\\uc138\\uc694\"");
    EXPECT_EQ(json.asString(), "안녕하세요");
  }
  {
    const auto json = json::parse("\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"");
    EXPECT_EQ(json.asString(), "\"\\/\b\f\n\r\t");
  }
}

TEST(dubu_json, number) {
#define test_case(x) EXPECT_DOUBLE_EQ(json::parse(#x).asNumber(), x)

  test_case(0.0);
  test_case(1.0);
  test_case(1.0e2);
  test_case(3.1415926535898);
  test_case(1.79769e+308);
  test_case(-1.79769e+308);
  test_case(-123e-2);

#undef test_case
}