#define CATCH_CONFIG_MAIN
#include "../../src/layout/layout.hpp"
#include <catch2/catch_all.hpp>

using namespace layout;

//////////// CUSTOM MATCHR //////////////

struct StyleNodeMatcher : Catch::Matchers::MatcherGenericBase {
    StyleNodeMatcher(const std::shared_ptr<StyleNode> expected) : expected_{expected} {}

    bool match(const std::shared_ptr<StyleNode> actual) const {
        if (actual->node_type != expected_->node_type) {
            return false;
        }

        if (actual->properties.size() != expected_->properties.size()) {
            return false;
        }

        for (const auto &elem : expected_->properties) {
            auto it = actual->properties.find(elem.first);
            if (it == actual->properties.end() || it->second != elem.second) {
                return false;
            }
        }

        for (int i = 0; i < actual->children.size(); i++) {
            StyleNodeMatcher snMatcher{expected_->children[i]};
            if (!snMatcher.match(actual->children[i]))
                return false;
        }

        return true;
    }

    std::string describe() const override {
        return "StyleNodes are equal";
    }

  private:
    const std::shared_ptr<StyleNode> expected_;
};

/////////////  TEST_CASE   /////////////////

TEST_CASE("make layout", "[layout]") {

    SECTION("layout1") {
        html::Node n{html::Element{"p", {{"id", "test"}, {"class", "testclass"}}, {}}};
        css::Selector s{css::Type::TypeSelector, css::TypeSelector{"p"}};
        REQUIRE(selector_matches(n, s) == true);
    }

    SECTION("layout2") {
        html::Node n{html::Element{"p", {{"id", "test"}, {"class", "testclass"}}, {}}};
        css::Selector s{css::Type::AttributeSelector,
                        css::AttributeSelector{"p", css::AttributeSelectorOp::eq, "id", "test"}};
        REQUIRE(selector_matches(n, s) == true);
    }

    SECTION("layout3") {
        html::Node n{html::Element{"p", {{"id", "test1 test2 test3"}, {"class", "testclass"}}, {}}};
        css::Selector s{
            css::Type::AttributeSelector,
            css::AttributeSelector{"p", css::AttributeSelectorOp::Contain, "id", "test2"}};
        REQUIRE(selector_matches(n, s) == true);
    }

    SECTION("layout4") {
        html::Node n{html::Element{"invalid", {{"id", "test"}, {"class", "testclass"}}, {}}};
        css::Selector s{css::Type::AttributeSelector,
                        css::AttributeSelector{"p", css::AttributeSelectorOp::eq, "id", "test"}};
        REQUIRE(selector_matches(n, s) == false);
    }

    SECTION("layout5") {
        html::Node n{html::Element{"p", {{"id", "test"}, {"class", "testclass"}}, {}}};
        css::Selector s{css::Type::ClassSelector, css::ClassSelector{"testclass"}};
        REQUIRE(selector_matches(n, s) == true);
    }
}

TEST_CASE("to_style_node", "[style node]") {

    SECTION("node1") {
        html::Node n{html::Element{"p", {{"id", "test"}, {"class", "testclass"}}, {}}};
        css::Selector s{css::Type::TypeSelector, css::TypeSelector{"p"}};
        std::vector<css::Selector> selectors{s};
        std::vector<css::Declaration> declarations{css::Declaration{"foo", "bar"},
                                                   css::Declaration{"piyo", "piyopiyo"}};
        std::vector<css::CSSRule> rules{css::CSSRule{selectors, declarations}};
        css::Stylesheet sheet{rules};

        std::unordered_map<std::string, std::string> properties{{"foo", "bar"},
                                                                {"piyo", "piyopiyo"}};

        auto expected =
            std::make_shared<StyleNode>(StyleNode{html::NodeType::Element, {}, properties});
        auto result = to_style_node(n, sheet);
        REQUIRE_THAT(result, StyleNodeMatcher(expected));
    }

    SECTION("display node") {
        html::Node n{html::Element{"p", {{"id", "test"}, {"class", "testclass"}}, {}}};

        css::Selector s{
            css::Type::AttributeSelector,
            css::AttributeSelector{"p", css::AttributeSelectorOp::eq, "class", "testclass"}};

        std::vector<css::Selector> selectors{s};
        std::vector<css::Declaration> declarations{css::Declaration{"display", "none"},
                                                   css::Declaration{"piyo", "piyopiyo"}};
        std::vector<css::CSSRule> rules{css::CSSRule{selectors, declarations}};
        css::Stylesheet sheet{rules};

        auto expected = nullptr;
        auto result = to_style_node(n, sheet);
        REQUIRE(expected == result);
    }
}
