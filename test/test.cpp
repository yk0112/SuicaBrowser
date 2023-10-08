#define CATCH_CONFIG_MAIN
#include "../src/htmlParser.hpp"
#include <catch2/catch_all.hpp>

// custom matcher for unorderd map
template <typename KeyType, typename ValueType>
struct UnorderdMapMatcher : Catch::Matchers::MatcherGenericBase {
    UnorderdMapMatcher(const std::unordered_map<KeyType, ValueType> &expected)
        : expected_{expected} {}

    bool match(const std::unordered_map<KeyType, ValueType> &actual) const {

        if (actual.size() != expected_.size()) {
            return false;
        }

        for (const auto &kv : expected_) {
            auto it = actual.find(kv.first);
            if (it == actual.end() || it->second != kv.second) {
                return false;
            }
        }
        return true;
    }

    std::string describe() const override {
        return "unordered maps are equal";
    }

  private:
    const std::unordered_map<KeyType, ValueType> &expected_;
};

// helper func for Attributes test
template <typename KeyType, typename ValueType>
auto UnorderdMapEquals(const std::unordered_map<KeyType, ValueType> &expected)
    -> UnorderdMapMatcher<KeyType, ValueType> {
    return UnorderdMapMatcher<KeyType, ValueType>(expected);
}

TEST_CASE("ParseAttribute", "[attribute]") {
    auto result1 = parse_attribute("test=\"foobar\"");
    REQUIRE(std::get<0>(result1) == "test");
    REQUIRE(std::get<1>(result1) == "foobar");

    auto result2 = parse_attribute("test = \"foobar\"");
    REQUIRE(std::get<0>(result2) == "test");
    REQUIRE(std::get<1>(result2) == "foobar");

    // auto result3 = parse_attribute("test = \"foo bar\"");
    // REQUIRE(std::get<0>(result2) == "test");
    // REQUIRE(std::get<1>(result2) == "foo bar");
}

TEST_CASE("Attributes", "[attribute]") {
    auto result1 = attributes("test=\"foobar\" abc=\"def\"");
    std::unordered_map<std::string, std::string> expected1{{"test", "foobar"}, {"abc", "def"}};
    REQUIRE_THAT(result1, UnorderdMapEquals(expected1));

    auto result2 = attributes("test = \"foobar\" abc = \"def\"");
    REQUIRE_THAT(result2, UnorderdMapEquals(expected1));

    auto result3 = attributes("");
    std::unordered_map<std::string, std::string> expected3{};
    REQUIRE_THAT(result3, UnorderdMapEquals(expected3));
}
