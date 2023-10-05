#define CATCH_CONFIG_MAIN
#include "../src/htmlParser.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Test Parse Attribute", "[attribute]") {
  auto result1 = parse_attribute("test=\"foobar\"");
  REQUIRE(result1.attributeName == "test");
  REQUIRE(result1.attributeValue == "foobar");

  auto result2 = parse_attribute("test = \"foobar\"");
  REQUIRE(result1.attributeName == "test");
  REQUIRE(result1.attributeValue == "foobar");
}

