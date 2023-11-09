#include "cssParser.hpp"
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace css {

namespace qi = boost::spirit::qi;

using Rule = qi::rule<std::string::iterator, std::string()>;
using Rule2 = qi::rule<std::string::iterator, Selector()>;

BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capUniv, cap_univ, 1)
BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capClass, cap_class, 1)
BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capAttr, cap_attr, 4)
BOOST_PHOENIX_ADAPT_FUNCTION(Selector, capType, cap_type, 1)

Rule nameRule = +(qi::char_ - ':');
Rule valueRule = +(qi::char_ - ';');
Rule declarationRule = nameRule >> qi::char_(':') >> valueRule;
Rule selectorRule = +(qi::char_ - ',');
Rule opRule = qi::string("~=") | qi::char_('=');

Rule2 universalRule = qi::as_string[qi::char_('*')][qi::_val = capUniv(qi::_1)];

Rule2 classRule = qi::lit('.') >> qi::as_string[+qi::char_][qi::_val = capClass(qi::_1)];

Rule2 typeRule = qi::as_string[*(qi::char_)][qi::_val = capType(qi::_1)];

Rule2 attributeRule =
    (qi::as_string[*(qi::char_ - '[' - ' ')] >> *qi::lit(' ') >> '[' >>
     qi::as_string[*(qi::char_ - opRule - ' ')] >> *qi::lit(' ') >> qi::as_string[opRule] >>
     *qi::lit(' ') >> qi::as_string[*(qi::char_ - ']' - ' ')] >> *qi::lit(' ') >>
     ']')[qi::_val = capAttr(qi::_1, qi::_2, qi::_3, qi::_4)];

Rule2 selectorsRule = universalRule | classRule | attributeRule | typeRule;

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

Declaration declaration(std::string input) {
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

std::vector<Declaration> declarations(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::vector<std::string> decls{};
    std::vector<Declaration> result{};

    bool sucsess =
        qi::phrase_parse(begin, end, (declarationRule || *(qi::space)) % ';', qi::space, decls);

    if (sucsess && begin == end) {
        for (auto &decl : decls) {
            result.push_back(declaration(decl));
        }
        return result;
    }
    throw std::runtime_error("fail to declarations");
}

std::vector<Selector> selectors(std::string input) {
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
        return CSSRule{selectors(selecs), declarations(decls)};
    }
    throw std::runtime_error("fail to rule");
}

} // namespace css
