#include "./htmlParser.hpp"

#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;

std::tuple<std::string, std::string> parse_attribute(std::string input) {
    auto begin = input.begin();
    auto end = input.end();
    std::string attributeName;
    std::string attributeValue;

    qi::rule<std::string::iterator, std::string()> attributeNameRule =
        +(qi::char_ - "=" - qi::space);
    qi::rule<std::string::iterator, std::string()> attributeValueRule =
        qi::lexeme['"' >> *(qi::char_ - '"') >> '"'];

    bool success = qi::phrase_parse(begin,
                                    end,
                                    attributeNameRule >> qi::lit("=") >> attributeValueRule,
                                    qi::space,
                                    attributeName,
                                    attributeValue);

    if (success && begin == end) {
        return std::make_tuple(attributeName, attributeValue);
        // return std::make_tuple("test","foo bar");
    }

    std::cout << "fail to parse attribute \n";
    exit(0);
}

AttrMap attributes(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::vector<std::string> attributes;
    AttrMap result;

    qi::rule<std::string::iterator, std::string()> attributeNameRule = +(qi::char_ - '=');
    qi::rule<std::string::iterator, std::string()> attributeValueRule =
        qi::lexeme[qi::char_('"') >> *(qi::char_ - '"') >> qi::char_('"')];

    qi::rule<std::string::iterator, std::string()> attributePair =
        attributeNameRule >> qi::char_("=") >> *qi::space >> attributeValueRule;

    if (input.empty()) {
        return result;
    }

    bool success = qi::parse(begin, end, attributePair % " ", attributes);

    if (success && begin == end) {
        for (auto &attr : attributes) {
            auto name_and_value = parse_attribute(attr);
            result.emplace(std::get<0>(name_and_value), std::get<1>(name_and_value));
        }

        return result;
    }

    std::cout << "fail to attributes \n";
    exit(0);
}
