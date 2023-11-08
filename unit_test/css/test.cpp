#define CATCH_CONFIG_MAIN
#include "../src/css/cssParser.hpp"
#include <catch2/catch_all.hpp>

using namespace css;

//////////// CUSTOM MATCHR //////////////

struct DeclarationsMatcher : Catch::Matchers::MatcherGenericBase {
    DeclarationsMatcher(const std::vector<Declaration> &expected) : expected_{expected} {}

    bool match(const std::vector<Declaration> &actual) const {

        if (actual.size() != expected_.size()) {
            return false;
        }

        for (int i = 0; i < expected_.size(); i++) {
            auto decl_a = actual[i];
            auto decl_e = expected_[i];

            if (decl_a.name != decl_e.name || decl_a.value != decl_a.value) {
                return false;
            }
        }

        return true;
    }

    std::string describe() const override {
        return "declarations are equal";
    }

  private:
    const std::vector<Declaration> &expected_;
};

struct SelectorMatcher : Catch::Matchers::MatcherGenericBase {
    SelectorMatcher(const Selector &expected) : expected_{expected} {}

    bool match(const Selector &actual) const {

        if (actual.type != expected_.type) {
            return false;
        }

        switch (actual.type) {
        case Type::UniversalSelector:
            return true;
        case Type::TypeSelector:
            return std::get<1>(actual.content).tag_name == std::get<1>(expected_.content).tag_name;
        case Type::AttributeSelector: {
            auto content1 = std::get<2>(actual.content);
            auto content2 = std::get<2>(expected_.content);

            if (content1.tag_name == content2.tag_name && content1.op == content2.op &&
                content1.attribute == content2.attribute && content1.value == content2.value)
                return true;
            else
                return false;
        }
        case Type::ClassSelector:
            return std::get<3>(actual.content).class_name ==
                   std::get<3>(expected_.content).class_name;
        default:
            return false;
        }
    }

    std::string describe() const override {
        return "Selector are equal";
    }

  private:
    const Selector &expected_;
};

struct SelectorsMatcher : Catch::Matchers::MatcherGenericBase {
    SelectorsMatcher(const std::vector<Selector> &expected) : expected_{expected} {}

    bool match(const std::vector<Selector> &actual) const {

        if (actual.size() != expected_.size()) {
            return false;
        }

        for (int i = 0; i < actual.size(); i++) {
            SelectorMatcher selectorMatcher{expected_[i]};
            if (!selectorMatcher.match(actual[i]))
                return false;
        }

        return true;
    }

    std::string describe() const override {
        return "declarations are equal";
    }

  private:
    const std::vector<Selector> &expected_;
};

/////////////  TEST_CASE   /////////////////

TEST_CASE("declarations", "[declarations]") {

    auto result1 = declarations("foo : bar; piyo : piyopiyo;");
    std::vector<Declaration> expected{Declaration{"foo", "bar"}, Declaration{"piyo", "piyopiyo"}};
    REQUIRE_THAT(result1, DeclarationsMatcher{expected});
}

TEST_CASE("selector", "[selector]") {

    SECTION("universal selector") {
        auto result = selectors("*");
        std::vector<Selector> expected{Selector{Type::UniversalSelector, UniversalSelector{}}};

        REQUIRE_THAT(result, SelectorsMatcher{expected});
    }

    SECTION("class selector") {
        auto result = selectors(".test");

        std::vector<Selector> expected{Selector{Type::ClassSelector, ClassSelector{"test"}}};

        REQUIRE_THAT(result, SelectorsMatcher{expected});
    }

    SECTION("multiple selectors") {
        auto result = selectors("test [foo = bar], a");

        auto content1 = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};
        auto content2 = TypeSelector{"a"};
        std::vector<Selector> expected{Selector{Type::AttributeSelector, content1},
                                       Selector{Type::TypeSelector, content2}};

        REQUIRE_THAT(result, SelectorsMatcher{expected});
    }
}
