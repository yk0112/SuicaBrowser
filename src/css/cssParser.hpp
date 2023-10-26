#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace css {

// Enumeration for AttributeSelectorOp
enum class AttributeSelectorOp {
    Eq,      // =
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

using Selector = std::variant<UniversalSelector, TypeSelector, AttributeSelector, ClassSelector>;

struct Declaration {
    std::string name;
    std::string value;
};

struct CSSRule {
    std::vector<Selector> selectors;
    std::vector<Declaration> declarations;
};

struct Stylesheet {
    std::vector<CSSRule> rules;
};

std::vector<Declaration> declarations(std::string input);

} // namespace css
