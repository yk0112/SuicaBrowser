#include "../layout/layout.hpp"
#include <boost/spirit/include/qi.hpp>

namespace layout {

namespace qi = boost::spirit::qi;

bool css_matches(const html::Node &node, const css::CSSRule &rule) {
    return std::any_of(rule.selectors.begin(),
                       rule.selectors.end(),
                       [node](const css::Selector &s) {
                           return layout::selector_matches(node, s);
                       });
};

bool selector_matches(const html::Node &n, const css::Selector &s) {
    switch (s.type) {

    case css::Type::UniversalSelector:
        return true;
    case css::Type::TypeSelector: {
        auto selector = std::get<1>(s.content);
        if (std::holds_alternative<html::Element>(n) &&
            selector.tag_name == std::get<1>(n).tag_name)
            return true;
        else
            return false;
    }
    case css::Type::AttributeSelector: {
        if (std::holds_alternative<html::Element>(n))
            return layout::AttributeSelector_matches(std::get<1>(n), s);
        else
            return false;
    }
    case css::Type::ClassSelector: {
        auto selector = std::get<3>(s.content);
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

bool AttributeSelector_matches(const html::Element &e, const css::Selector &s) {

    auto selector = std::get<2>(s.content);

    if (!(selector.tag_name == e.tag_name))
        return false;

    if (selector.op == css::AttributeSelectorOp::eq && e.attributes.contains(selector.attribute)) {
        try {
            return e.attributes.at(selector.attribute) == selector.value;
        } catch (const std::out_of_range &) {
            return false;
        }
    }

    if (selector.op == css::AttributeSelectorOp::Contain &&
        e.attributes.contains(selector.attribute)) {
        auto attr = e.attributes.at(selector.attribute);
        std::vector<std::string> values;
        auto begin = attr.begin();
        auto end = attr.end();

        auto sucsess = qi::parse(begin, end, *(qi::char_ - ' ') % qi::space, values);

        if (sucsess && begin == end)
            return std::find(values.begin(), values.end(), selector.value) != values.end();
        else
            return false;
    }
    return false;
};

} // namespace layout
