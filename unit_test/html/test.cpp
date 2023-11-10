#define CATCH_CONFIG_MAIN
#include "../src/html/htmlParser.hpp"
#include <catch2/catch_all.hpp>

//////////// CUSTOM MATCHR //////////////

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

struct NodeMatcher;

struct ElementMatcher : Catch::Matchers::MatcherGenericBase {
    ElementMatcher(const html::Element &expected) : expected_{expected} {}

    bool match(const html::Element &actual) const;

    std::string describe() const override {
        return "Elements are equal";
    }

  private:
    const html::Element &expected_;
};

struct NodeMatcher : Catch::Matchers::MatcherGenericBase {
    NodeMatcher(const html::Node &expected) : expected_{expected} {}

    bool match(const html::Node &actual) const {
        if (actual.index() != expected_.index()) {
            return false;
        }

        if (actual.index() == 0) {
            return std::get<0>(actual) == std::get<0>(expected_);
        } else if (actual.index() == 1) {
            ElementMatcher elementMatcher{std::get<1>(expected_)};
            return elementMatcher.match(std::get<1>(actual));
        }
        return false;
    }

    std::string describe() const override {
        return "Nodes are equal";
    }

  private:
    const html::Node &expected_;
};

bool ElementMatcher::match(const html::Element &actual) const {
    if (actual.tag_name != expected_.tag_name) {
        return false;
    }

    UnorderdMapMatcher mapMatcher{expected_.attributes};
    if (!mapMatcher.match(actual.attributes))
        return false;

    if (actual.children.size() != expected_.children.size())
        return false;

    for (int i = 0; i < actual.children.size(); i++) {
        NodeMatcher nodeMatcher{expected_.children[i]};
        if (!nodeMatcher.match(actual.children[i]))
            return false;
    }
    return true;
}

// helper func for Attributes test
template <typename KeyType, typename ValueType>
auto UnorderdMapEquals(const std::unordered_map<KeyType, ValueType> &expected)
    -> UnorderdMapMatcher<KeyType, ValueType> {
    return UnorderdMapMatcher<KeyType, ValueType>{expected};
}

// helper func for Node matcher
NodeMatcher NodeEquals(const html::Node &expected) {
    return NodeMatcher{expected};
}

// helper func for Element matcher
ElementMatcher Elementquals(const html::Element &expected) {
    return ElementMatcher{expected};
}

/////////////  TEST_CASE   /////////////////

TEST_CASE("parse_attribute", "[tag]") {

    auto result1 = html::parse_attribute("test=\"foobar\"");
    REQUIRE(std::get<0>(result1) == "test");
    REQUIRE(std::get<1>(result1) == "foobar");

    auto result2 = html::parse_attribute("test = \"foobar\"");
    REQUIRE(std::get<0>(result2) == "test");
    REQUIRE(std::get<1>(result2) == "foobar");

    auto result3 = html::parse_attribute("test = \"\"");
    REQUIRE(std::get<0>(result3) == "test");
    REQUIRE(std::get<1>(result3) == "");

    // auto result3 = parse_attribute("test = \"foo bar\"");
    // REQUIRE(std::get<0>(result2) == "test");
    // REQUIRE(std::get<1>(result2) == "foo bar");
}

TEST_CASE("parse_attributes", "[tag]") {

    auto result1 = html::parse_attributes("test=\"foobar\" abc=\"def\"");
    std::unordered_map<std::string, std::string> expected1{{"test", "foobar"}, {"abc", "def"}};
    REQUIRE_THAT(result1, UnorderdMapEquals(expected1));

    auto result2 = html::parse_attributes("test = \"foobar\" abc = \"def\"");
    REQUIRE_THAT(result2, UnorderdMapEquals(expected1));

    auto result3 = html::parse_attributes("");
    std::unordered_map<std::string, std::string> expected3{};
    REQUIRE_THAT(result3, UnorderdMapEquals(expected3));
}

TEST_CASE("open_tag", "[tag]") {

    SECTION("tag without attributes") {
        std::string str = "<p>aaaaaa";
        auto begin = str.begin();
        auto end = str.end();
        auto result = html::parse_open_tag(begin, end);
        std::unordered_map<std::string, std::string> expected{};
        REQUIRE(std::get<0>(result) == "p");
        REQUIRE_THAT(std::get<1>(result), UnorderdMapEquals(expected));
        //  REQUIRE(std::get<2>(result) == "aaaaaa");
    }

    SECTION("tag with a attribute") {
        std::string str = "<p id=\"test\">";
        auto begin = str.begin();
        auto end = str.end();

        auto result = html::parse_open_tag(begin, end);
        std::unordered_map<std::string, std::string> expected{{"id", "test"}};
        REQUIRE(std::get<0>(result) == "p");
        REQUIRE_THAT(std::get<1>(result), UnorderdMapEquals(expected));
    }

    SECTION("tag with multiple attributes") {
        std::string str = "<p id=\"test\" class=\"sample\">";
        auto begin = str.begin();
        auto end = str.end();

        auto result = html::parse_open_tag(begin, end);
        std::unordered_map<std::string, std::string> expected{{"id", "test"}, {"class", "sample"}};
        REQUIRE(std::get<0>(result) == "p");
        REQUIRE_THAT(std::get<1>(result), UnorderdMapEquals(expected));
    }
    SECTION("multiple tags") {
        std::string str = "<div id=\"test\"><p>hello world</p></div>";
        auto begin = str.begin();
        auto end = str.end();

        auto result = html::parse_open_tag(begin, end);
        std::unordered_map<std::string, std::string> expected{{"id", "test"}};
        REQUIRE(std::get<0>(result) == "div");
        REQUIRE_THAT(std::get<1>(result), UnorderdMapEquals(expected));
    }

    SECTION("invalid tag") {
        std::string str = "<p id>";
        auto begin = str.begin();
        auto end = str.end();
        REQUIRE_THROWS(html::parse_open_tag(begin, end));
    }

    SECTION("invalid tag") {
        std::string str = "</p>";
        auto begin = str.begin();
        auto end = str.end();
        REQUIRE_THROWS(html::parse_open_tag(begin, end));
    }
}

TEST_CASE("close_tag", "[tag]") {
    std::string str = "</p>";
    auto begin = str.begin();
    auto end = str.end();
    auto result = html::parse_close_tag(begin, end);

    REQUIRE(result == "p");
}

TEST_CASE("parse_text", "[tag]") {
    std::string str = "aaaaaa</p>";
    auto begin = str.begin();
    auto end = str.end();
    auto result = html::parse_text(begin, end);
    REQUIRE(result == "aaaaaa");

    std::string str2 = "</p>";
    begin = str2.begin();
    end = str2.end();
    REQUIRE(html::parse_text(begin, end) == "");
}

TEST_CASE("parse_element", "[element]") {
    SECTION("empty element") {
        std::string input = "<p></p>";
        auto begin = input.begin();
        auto end = input.end();

        html::AttrMap attributes{};
        std::vector<html::Node> children{html::Node{}};
        html::Element expected{"p", attributes, children};
        auto result = html::parse(begin, end);
        REQUIRE_THAT(result, Elementquals(expected));
    }
    SECTION("ement with text") {
        std::string input = "<p>hello world</p>";
        auto begin = input.begin();
        auto end = input.end();

        html::AttrMap attributes{};
        std::vector<html::Node> children{html::Node{"hello world"}};
        html::Element expected{"p", attributes, children};
        auto result = html::parse(begin, end);
        REQUIRE_THAT(result, Elementquals(expected));
    }
    SECTION("nested elements") {
        std::string input = "<div class=\"sample\"><p id=\"test\">hello world</p></div>";
        auto begin = input.begin();
        auto end = input.end();

        html::AttrMap attributes{{"id", "test"}};
        std::vector<html::Node> children{html::Node{"hello world"}};
        html::Element childElement{"p", attributes, children};

        html::AttrMap attributes2{{"class", "sample"}};
        std::vector<html::Node> children2{html::Node{childElement}};
        html::Element expected{"div", attributes2, children2};

        auto result = html::parse(begin, end);
        REQUIRE_THAT(result, Elementquals(expected));
    }

    //   missmatchの場合は、parse_element内でプログラムを終了させる
    /*   SECTION("mismatched tags") {
          std::string input = "<p>hello world</div>";
          auto begin = input.begin();
          auto end = input.end();
          REQUIRE_THROWS(html::parse(begin, end));
      } */
}
