#include "./htmlParser.hpp"
#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <string>

std::tuple<std::string, std::string> TagParser::parse_attribute(std::string input) {
    auto begin = input.begin();
    auto end = input.end();
    std::string attributeName;
    std::string attributeValue;

    bool success = qi::phrase_parse(begin,
                                    end,
                                    attributeNameRule >> qi::lit("=") >> parseValueRule,
                                    qi::space,
                                    attributeName,
                                    attributeValue);

    if (success && begin == end) {
        return std::make_tuple(attributeName, attributeValue);
    }

    throw std::runtime_error("fail to parse_attribute");
}

AttrMap TagParser::parse_attributes(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::cout << input << "\n";
    std::vector<std::string> attributes;
    AttrMap result;

    if (input.empty()) {
        return result;
    }

    bool success =
        qi::phrase_parse(begin, end, attributeRule % *qi::char_(' '), qi::space, attributes);

    if (success && begin == end) {
        for (auto &attr : attributes) {
            auto name_and_value = parse_attribute(attr);
            result.emplace(std::get<0>(name_and_value), std::get<1>(name_and_value));
        }

        return result;
    }

    throw std::runtime_error("fail to parse_attributes");
}

std::pair<std::string, AttrMap> TagParser::open_tag(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::string tagName;
    std::string attributes;

    bool success = qi::parse(begin,
                             end,
                             '<' >> tagNameRule >> *qi::lit(' ') >>
                                 (tagAttributesRule | qi::attr("")) >> '>' >> *qi::char_,
                             tagName,
                             attributes);

    if (success && begin == end) {
        auto attrs_map = parse_attributes(attributes);
        return std::make_pair(tagName, attrs_map);
    }

    throw std::runtime_error("fail to open_tag");
}

