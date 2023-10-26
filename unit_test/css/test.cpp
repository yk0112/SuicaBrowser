#define CATCH_CONFIG_MAIN
#include "../src/css/cssParser.hpp"
#include <catch2/catch_all.hpp>

//////////// CUSTOM MATCHR //////////////

// custom matcher for unorderd map
struct DeclarationsMatcher : Catch::Matchers::MatcherGenericBase {
    DeclarationsMatcher(const std::vector<css::Declaration> &expected) : expected_{expected} {}

    bool match(const std::vector<css::Declaration> &actual) const {

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
    const std::vector<css::Declaration> &expected_;
};

// helper func for declarations test

DeclarationsMatcher declarationsEquals(const std::vector<css::Declaration> &expected) {
    return DeclarationsMatcher{expected};
}

/////////////  TEST_CASE   /////////////////

TEST_CASE("declarations", "[declarations]") {

    auto result1 = css::declarations("foo : bar; piyo : piyopiyo;");
    std::vector<css::Declaration> expected{css::Declaration{"foo", "bar"},
                                           css::Declaration{"piyo", "piyopiyo"}};
    REQUIRE_THAT(result1, declarationsEquals(expected));
}
