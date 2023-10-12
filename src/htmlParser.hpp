#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using AttrMap = std::unordered_map<std::string, std::string>;

enum class NodeType { Element, Text };

struct Text {
    std::string data;
};

struct Node {
    NodeType node_type;
    std::vector<std::unique_ptr<Node>> children;
};

struct Element {
    std::string tag_name;
    AttrMap attributes;
};

namespace qi = boost::spirit::qi;

class TagParser {
  public:
    TagParser() {
        parseValueRule = '"' >> *(qi::char_ - '"') >> '"';
        attributeNameRule = +(qi::char_ - '=' - ' ');
        attributeValueRule = qi::char_('"') >> *(qi::char_ - '"') >> qi::char_('"');

        attributeRule =
            attributeNameRule >> *qi::space >> qi::char_("=") >> *qi::space >> attributeValueRule;

        tagNameRule = +(qi::char_ - ' ' - '>');
        tagAttributesRule = +(qi::char_ - '>');
    }

    std::tuple<std::string, std::string> parse_attribute(std::string input);

    AttrMap parse_attributes(std::string input);

    std::pair<std::string, AttrMap> open_tag(std::string input);

  private:
    qi::rule<std::string::iterator, std::string()> parseValueRule;
    qi::rule<std::string::iterator, std::string()> attributeNameRule;
    qi::rule<std::string::iterator, std::string()> attributeValueRule;
    qi::rule<std::string::iterator, std::string()> attributeRule;
    qi::rule<std::string::iterator, std::string()> tagNameRule;
    qi::rule<std::string::iterator, std::string()> tagAttributesRule;
    qi::rule<std::string::iterator, std::string()> tagRule;
};

