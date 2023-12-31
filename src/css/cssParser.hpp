// #include "../html/htmlParser.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace css {

enum class AttributeSelectorOp {
    eq,      // =
    Contain, // ~=
};

struct UniversalSelector {};

struct TypeSelector {
    std::string tag_name;
};

struct AttributeSelector {
    std::string tag_name;
    AttributeSelectorOp op;
    std::string attribute;
    std::string value;
};

struct ClassSelector {
    std::string class_name;
};

enum class Type {
    UniversalSelector,
    ClassSelector,
    TypeSelector,
    AttributeSelector,
};

using Content = std::variant<UniversalSelector, TypeSelector, AttributeSelector, ClassSelector>;

struct Selector {
    Type type;
    Content content;

    //    bool matches(const html::Node &n) const;

    //    bool AttributeSelector_matches(const html::Element &e) const;
};

struct Declaration {
    std::string name;
    std::string value;
};

struct CSSRule {
    std::vector<Selector> selectors;
    std::vector<Declaration> declarations;

    //    bool matches(const html::Node &n) const;
};

struct Stylesheet {
    std::vector<CSSRule> rules;
};

Selector cap_univ(std::string s);

Selector cap_class(std::string s);

Selector cap_type(std::string s);

Selector cap_attr(std::string s1, std::string s2, std::string s3, std::string s4);

CSSRule cap_CSSRule(std::string s1);

std::vector<Declaration> parse_declarations(std::string input);

std::vector<Selector> parse_selectors(std::string input);

CSSRule parse_rule(std::string input);

std::vector<CSSRule> parse(std::string input);

} // namespace css
