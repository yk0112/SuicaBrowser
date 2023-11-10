#include "htmlParser.hpp"

namespace html {

namespace qi = boost::spirit::qi;

using Rule = qi::rule<std::string::iterator, std::string()>;

Rule value = '"' >> *(qi::char_ - '"') >> '"';
Rule attributeName = +(qi::char_ - '=' - ' ');
Rule attributeValue = qi::char_('"') >> *(qi::char_ - '"') >> qi::char_('"');
Rule attribute = attributeName >> *qi::space >> qi::char_("=") >> *qi::space >> attributeValue;

Rule tagName = +(qi::char_("a-zA-Z") - ' ' - '>');
Rule tagAttributes = +(qi::char_ - '>');
Rule textContent = *(qi::char_ - '<');

std::tuple<std::string, std::string> parse_attribute(std::string input) {
    auto begin = input.begin();
    auto end = input.end();
    std::string name{};
    std::string attrValue{};

    bool success = qi::phrase_parse(begin,
                                    end,
                                    attributeName >> qi::lit("=") >> value,
                                    qi::space,
                                    name,
                                    attrValue);

    if (success && begin == end) {
        return std::make_tuple(name, attrValue);
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

    bool success = qi::phrase_parse(begin, end, attribute % *qi::char_(' '), qi::space, attributes);

    if (success && begin == end) {
        for (auto &attr : attributes) {
            auto name_and_value = parse_attribute(attr);
            result.emplace(std::get<0>(name_and_value), std::get<1>(name_and_value));
        }

        return result;
    }

    throw std::runtime_error("fail to parse_attributes");
}

std::tuple<std::string, AttrMap> parse_open_tag(std::string::iterator &begin,
                                                std::string::iterator &end) {
    std::string name{};
    std::string attributes{};
    std::string rest{};

    bool success =
        qi::parse(begin,
                  end,
                  '<' >> tagName >> *qi::lit(' ') >> (tagAttributes | qi::attr("")) >> '>',
                  name,
                  attributes);

    if (success) {
        auto attrs_map = parse_attributes(attributes);
        return std::make_tuple(name, attrs_map);
    }

    throw std::runtime_error("fail to open_tag");
}

std::string parse_close_tag(std::string::iterator &begin, std::string::iterator &end) {

    std::string name{};

    bool success = qi::parse(begin, end, "</" >> tagName >> '>', name);
    if (success) {
        return name;
    }

    throw std::runtime_error("fail to close tag");
}

html::Element parse_element(std::string &input);

std::string parse_text(std::string::iterator &begin, std::string::iterator &end) {
    std::string result{};
    bool sucsess = qi::parse(begin, end, textContent, result);
    if (sucsess) {
        return result;
    }
    throw std::runtime_error("fail to parse_text");
}

std::vector<html::Node> parse_nodes(std::string::iterator &begin, std::string::iterator &end) {
    std::vector<html::Node> result{};
    std::string::iterator begin_tmp;
    do {
        try {
            begin_tmp = begin;
            result.push_back(html::Node{parse(begin, end)});
        } catch (const std::exception &ex) {
            begin = begin_tmp;
            result.push_back(html::Node{parse_text(begin, end)});
        }
    } while (*begin != '<' || *(begin + 1) != '/');

    return result;
}

html::Element parse(std::string::iterator &begin, std::string::iterator &end) {
    try {
        auto result1 = html::parse_open_tag(begin, end);
        auto children = html::parse_nodes(begin, end);
        auto result2 = html::parse_close_tag(begin, end);

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
