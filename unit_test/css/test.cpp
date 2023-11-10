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
                content1.attribute == content2.attribute && content1.value == content2.value) {
                return true;
            } else
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
        return "Selectors are equal";
    }

  private:
    const std::vector<Selector> &expected_;
};

struct RulesMatcher : Catch::Matchers::MatcherGenericBase {
    RulesMatcher(const std::vector<CSSRule> &expected) : expected_{expected} {}

    bool match(const std::vector<CSSRule> &actual) const {

        if (actual.size() != expected_.size()) {
            return false;
        }

        for (int i = 0; i < actual.size(); i++) {
            SelectorsMatcher selectorsMatcher{expected_[i].selectors};
            DeclarationsMatcher declMatcher{expected_[i].declarations};

            if (!selectorsMatcher.match(actual[i].selectors) ||
                !declMatcher.match(actual[i].declarations))
                return false;
        }

        return true;
    }

    std::string describe() const override {
        return "Rules are equal";
    }

  private:
    const std::vector<CSSRule> &expected_;
};

/////////////  TEST_CASE   /////////////////

TEST_CASE("declarations", "[declarations]") {

    auto result1 = parse_declarations("foo : bar; piyo : piyopiyo;");
    std::vector<Declaration> expected{Declaration{"foo", "bar"}, Declaration{"piyo", "piyopiyo"}};
    REQUIRE_THAT(result1, DeclarationsMatcher{expected});

    auto result2 = parse_declarations("");
    std::vector<Declaration> expected2{};
    REQUIRE_THAT(result2, DeclarationsMatcher{expected2});
}

TEST_CASE("selector", "[selector]") {

    SECTION("universal selector") {
        auto result = parse_selectors("*");
        std::vector<Selector> expected{Selector{Type::UniversalSelector, UniversalSelector{}}};

        REQUIRE_THAT(result, SelectorsMatcher{expected});
    }

    SECTION("class selector") {
        auto result = parse_selectors(".test");

        std::vector<Selector> expected{Selector{Type::ClassSelector, ClassSelector{"test"}}};

        REQUIRE_THAT(result, SelectorsMatcher{expected});
    }

    SECTION("multiple selectors") {
        auto result = parse_selectors("test[foo=bar],a");

        auto content1 = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};
        auto content2 = TypeSelector{"a"};
        std::vector<Selector> expected{Selector{Type::AttributeSelector, content1},
                                       Selector{Type::TypeSelector, content2}};

        REQUIRE_THAT(result, SelectorsMatcher{expected});
    }

    SECTION("multiple attributes") {
        auto result = parse_selectors("test [foo=bar], testtest[piyo~=guoo]");

        auto content1 = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};
        auto content2 = AttributeSelector{"testtest", AttributeSelectorOp::Contain, "piyo", "guoo"};

        std::vector<Selector> expected{Selector{Type::AttributeSelector, content1},
                                       Selector{Type::AttributeSelector, content2}};

        REQUIRE_THAT(result, SelectorsMatcher{expected});
    }
}

TEST_CASE("parse_rule", "[rule]") {

    SECTION("empty declarations") {
        auto result = parse_rule("test [foo=bar] {}");

        auto content1 = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};
        std::vector<Selector> selecs{Selector{Type::AttributeSelector, content1}};
        std::vector<Declaration> decls{};

        REQUIRE_THAT(result.selectors, SelectorsMatcher{selecs});
        REQUIRE_THAT(result.declarations, DeclarationsMatcher{decls});
    }

    SECTION("multiple selectors") {
        auto result = parse_rule("test [foo=bar], testtest[piyo~=guoo] {}");

        auto content1 = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};
        auto content2 = AttributeSelector{"testtest", AttributeSelectorOp::Contain, "piyo", "guoo"};

        std::vector<Selector> selecs{Selector{Type::AttributeSelector, content1},
                                     Selector{Type::AttributeSelector, content2}};

        std::vector<Declaration> decls{};

        REQUIRE_THAT(result.selectors, SelectorsMatcher{selecs});
        REQUIRE_THAT(result.declarations, DeclarationsMatcher{decls});
    }

    SECTION("selecs and decls") {
        auto result = parse_rule("test [foo=bar] { aa: bb; cc: dd; }");

        auto content = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};

        std::vector<Selector> selecs{Selector{Type::AttributeSelector, content}};

        std::vector<Declaration> decls{Declaration{"aa", "bb"}, Declaration{"cc", "dd"}};

        REQUIRE_THAT(result.selectors, SelectorsMatcher{selecs});
        REQUIRE_THAT(result.declarations, DeclarationsMatcher{decls});
    }
}

TEST_CASE("parse_rules", "[rule]") {

    SECTION("single rule") {
        auto result = parse("test [foo=bar] { aa: bb; cc: dd; }");

        auto content1 = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};
        std::vector<Selector> selecs1{Selector{Type::AttributeSelector, content1}};
        std::vector<Declaration> decls1{Declaration{"aa", "bb"}, Declaration{"cc", "dd"}};
        CSSRule rule1{selecs1, decls1};

        std::vector<CSSRule> expected{rule1};

        REQUIRE_THAT(result, RulesMatcher{expected});
    }

    SECTION("multiple rules") {
        auto result = parse("test [foo=bar] { aa: bb; cc: dd; } \
	                           test2[piyo = piyopiyo] { ee: ff; gg: hh; }");

        auto content1 = AttributeSelector{"test", AttributeSelectorOp::eq, "foo", "bar"};
        std::vector<Selector> selecs1{Selector{Type::AttributeSelector, content1}};
        std::vector<Declaration> decls1{Declaration{"aa", "bb"}, Declaration{"cc", "dd"}};
        CSSRule rule1{selecs1, decls1};

        auto content2 = AttributeSelector{"test2", AttributeSelectorOp::eq, "piyo", "piyopiyo"};
        std::vector<Selector> selecs2{Selector{Type::AttributeSelector, content2}};
        std::vector<Declaration> decls2{Declaration{"ee", "ff"}, Declaration{"gg", "hh"}};
        CSSRule rule2{selecs2, decls2};

        std::vector<CSSRule> expected{rule1, rule2};

        REQUIRE_THAT(result, RulesMatcher{expected});
    }
}
