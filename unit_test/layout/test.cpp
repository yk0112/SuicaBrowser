#define CATCH_CONFIG_MAIN
#include "../../src/layout/layout.hpp"
#include <catch2/catch_all.hpp>

using namespace layout;

//////////// CUSTOM MATCHR //////////////

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
