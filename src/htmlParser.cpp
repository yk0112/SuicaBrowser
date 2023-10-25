#include "htmlParser.hpp"

namespace html {

namespace qi = boost::spirit::qi;

using Rule = qi::rule<std::string::iterator, std::string()>;

Rule parseValueRule = '"' >> *(qi::char_ - '"') >> '"';
Rule attributeNameRule = +(qi::char_ - '=' - ' ');
Rule attributeValueRule = qi::char_('"') >> *(qi::char_ - '"') >> qi::char_('"');
Rule attributeRule = attributeNameRule >> *qi::space >> qi::char_("=") >> *qi::space
                     >> attributeValueRule;

Rule tagNameRule = +(qi::char_("a-zA-Z") - ' ' - '>');
Rule tagAttributesRule = +(qi::char_ - '>');
Rule textContentRule = *(qi::char_ - '<');

std::tuple<std::string, std::string> parse_attribute(std::string input) {
    auto begin = input.begin();
    auto end = input.end();
    std::string attributeName{};
    std::string attributeValue{};

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

AttrMap parse_attributes(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::vector<std::string> attributes{};
    AttrMap result{};

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

std::tuple<std::string, AttrMap> open_tag(std::string::iterator &begin,
                                          std::string::iterator &end) {
    std::string tagName{};
    std::string attributes{};
    std::string rest{};

    bool success =
        qi::parse(begin,
                  end,
                  '<' >> tagNameRule >> *qi::lit(' ') >> (tagAttributesRule | qi::attr("")) >> '>',
                  tagName,
                  attributes);

    if (success) {
        auto attrs_map = parse_attributes(attributes);
        return std::make_tuple(tagName, attrs_map);
    }

    throw std::runtime_error("fail to open_tag");
}

std::string close_tag(std::string::iterator &begin, std::string::iterator &end) {

    std::string tagName{};

    bool success = qi::parse(begin, end, "</" >> tagNameRule >> '>', tagName);
    if (success) {
        return tagName;
    }

    throw std::runtime_error("fail to close tag");
}

html::Element parse_element(std::string &input);

std::string parse_text(std::string::iterator &begin, std::string::iterator &end) {
    std::string result{};
    bool sucsess = qi::parse(begin, end, textContentRule, result);
    if (sucsess) {
        return result;
    }
    throw std::runtime_error("fail to parse_text");
}

std::vector<html::Node> nodes(std::string::iterator &begin, std::string::iterator &end) {
    std::vector<html::Node> result{};
    std::string::iterator begin_tmp;
    do {
        try {
            begin_tmp = begin;
            result.push_back(html::Node{parse_element(begin, end)});
        } catch (const std::exception &ex) {
            begin = begin_tmp;
            result.push_back(html::Node{parse_text(begin, end)});
        }
    } while (*begin != '<' || *(begin + 1) != '/');

    return result;
}

html::Element parse_element(std::string::iterator &begin, std::string::iterator &end) {
    try {
        auto result1 = html::open_tag(begin, end);
        auto children = html::nodes(begin, end);
        auto result2 = html::close_tag(begin, end);

        if (std::get<0>(result1) == result2) {
            return html::Element{std::get<0>(result1), std::get<1>(result1), children};
        }

        std::cerr << "missmatch tags" << std::endl;
        exit(1);

    } catch (const std::exception &) {

        throw std::runtime_error("fail to parse_element");
    }
}
} // namespace html
