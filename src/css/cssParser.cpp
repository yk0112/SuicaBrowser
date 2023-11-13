#include "cssParser.hpp"
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace css {

namespace qi = boost::spirit::qi;

//////  CSS rule for parse  //////

using Rule = qi::rule<std::string::iterator, std::string()>;
using Rule2 = qi::rule<std::string::iterator, Selector()>;
using Rule3 = qi::rule<std::string::iterator, CSSRule()>;

BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capUniv, cap_univ, 1)
BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capClass, cap_class, 1)
BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capAttr, cap_attr, 4)
BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capType, cap_type, 1)
BOOST_PHOENIX_ADAPT_FUNCTION(CSSRule, capCSSRule, cap_CSSRule, 1)

const Rule name = +(qi::char_ - ':');
const Rule value = +(qi::char_ - ';');
const Rule declaration = name >> qi::char_(':') >> value;
const Rule selector = +(qi::char_ - ',');
const Rule op = qi::string("~=") | qi::char_('=');

const Rule2 universal_selector = qi::as_string[qi::char_('*')][qi::_val = capUniv(qi::_1)];

const Rule2 class_selector = qi::lit('.') >> qi::as_string[+qi::char_][qi::_val = capClass(qi::_1)];

const Rule2 type_selector = qi::as_string[*(qi::char_)][qi::_val = capType(qi::_1)];

const Rule2 attribute_selector =
    (qi::as_string[*(qi::char_ - '[' - ' ')] >> *qi::lit(' ') >> '[' >>
     qi::as_string[*(qi::char_ - op - ' ')] >> *qi::lit(' ') >> qi::as_string[op] >>
     *qi::lit(' ') >> qi::as_string[*(qi::char_ - ']' - ' ')] >> *qi::lit(' ') >>
     ']')[qi::_val = capAttr(qi::_1, qi::_2, qi::_3, qi::_4)];

const Rule2 selectorsRule =
    universal_selector | class_selector | attribute_selector | type_selector;

const Rule3 cssRule = qi::as_string[+(qi::char_ - '{') >> qi::char_('{') >> *(qi::char_ - '}') >>
                                    qi::char_('}')][qi::_val = capCSSRule(qi::_1)];

Selector cap_univ(std::string s) {
    return Selector{Type::UniversalSelector, UniversalSelector{}};
};

Selector cap_class(std::string s) {
    return Selector{Type::ClassSelector, ClassSelector{s}};
};

Selector cap_type(std::string s) {
    return Selector{Type::TypeSelector, TypeSelector{s}};
};

Selector cap_attr(std::string s1, std::string s2, std::string s3, std::string s4) {
    if (s3 == "=") {
        return Selector{Type::AttributeSelector,
                        AttributeSelector{s1, AttributeSelectorOp::eq, s2, s4}};
    } else {
        return Selector{Type::AttributeSelector,
                        AttributeSelector{s1, AttributeSelectorOp::Contain, s2, s4}};
    }
};

CSSRule cap_CSSRule(std::string s1) {
    return parse_rule(s1);
};

//////// matchers for rendering tree  //////////
/* bool CSSRule::matches(const html::Node &n) const {
    return std::any_of(this->selectors.begin(), this->selectors.end(), [n](const Selector &s) {
        return s.matches(n);
    });
};

bool Selector::AttributeSelector_matches(const html::Element &element) const {

    auto selector = std::get<2>(this->content);

    if (!(selector.tag_name == element.tag_name))
        return false;

    if (selector.op == AttributeSelectorOp::eq && element.attributes.contains(selector.attribute)) {
        try {
            return element.attributes.at(selector.attribute) == selector.value;
        } catch (const std::out_of_range &) {
            return false;
        }
    }

    if (selector.op == AttributeSelectorOp::Contain &&
        element.attributes.contains(selector.attribute)) {
        auto attr = element.attributes.at(selector.attribute);
        std::vector<std::string> values;
        auto begin = attr.begin();
        auto end = attr.end();
        auto sucsess = qi::parse(begin, end, *qi::char_ % *qi::space, values);

        if (sucsess && begin == end)
            return std::find(values.begin(), values.end(), selector.value) != values.end();
        else
            return false;
    }
    return false;
}

bool Selector::matches(const html::Node &n) const {
    switch (this->type) {

    case Type::UniversalSelector:
        return true;
    case Type::TypeSelector: {
        auto selector = std::get<1>(this->content);
        if (std::holds_alternative<html::Element>(n) &&
            selector.tag_name == std::get<1>(n).tag_name)
            return true;
        else
            return false;
    }
    case Type::AttributeSelector: {
        if (std::holds_alternative<html::Element>(n))
            return AttributeSelector_matches(std::get<1>(n));
        else
            return false;
    }
    case Type::ClassSelector: {
        auto selector = std::get<3>(this->content);
        if (std::holds_alternative<html::Element>(n)) {
            auto element = std::get<1>(n);
            try {
                return element.attributes.at("class") == selector.class_name;
            } catch (const std::out_of_range &) {
                return false;
            }
        }
        return false;
    }
    default:
        return false;
    }
};
 */
////////parse function //////////////

Declaration parse_declaration(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::string name{};
    std::string value{};

    bool sucsess = qi::phrase_parse(begin,
                                    end,
                                    *(qi::char_ - ':' - ' ') >> ':' >> *(qi::char_ - ' '),
                                    qi::space,
                                    name,
                                    value);

    if (sucsess && begin == end) {
        return Declaration{name, value};
    }

    throw std::runtime_error("fail to declaration");
}

std::vector<Declaration> parse_declarations(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::vector<std::string> decls{};
    std::vector<Declaration> result{};

    bool sucsess =
        qi::phrase_parse(begin, end, (declaration || *(qi::space)) % ';', qi::space, decls);

    if (sucsess && begin == end) {
        for (auto &decl : decls) {
            result.push_back(parse_declaration(decl));
        }
        return result;
    }
    throw std::runtime_error("fail to declarations");
}

std::vector<Selector> parse_selectors(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::vector<Selector> result{};

    bool sucsess = qi::phrase_parse(begin, end, selectorsRule % ',', qi::space, result);

    if (sucsess && begin == end) {
        return result;
    }

    throw std::runtime_error("fail to selectors");
}

CSSRule parse_rule(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::string decls{};
    std::string selecs{};

    bool sucsess = qi::phrase_parse(begin,
                                    end,
                                    +(qi::char_ - '{') >> '{' >> *(qi::char_ - '}') >> '}',
                                    qi::space,
                                    selecs,
                                    decls);

    if (sucsess && begin == end) {
        return CSSRule{parse_selectors(selecs), parse_declarations(decls)};
    }
    throw std::runtime_error("fail to rule");
}

std::vector<CSSRule> parse(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::vector<CSSRule> result{};

    bool sucsess = qi::phrase_parse(begin, end, cssRule % *qi::space, qi::space, result);

    if (sucsess && begin == end) {
        return result;
    }
    throw std::runtime_error("fail to parse_rules");
}

} // namespace css
