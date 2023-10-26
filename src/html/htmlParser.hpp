#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace html {

using AttrMap = std::unordered_map<std::string, std::string>;

struct Text {
    std::string data;
};

struct Element;

using Node = std::variant<std::string, Element>;

struct Element {
    std::string tag_name;
    AttrMap attributes;
    std::vector<Node> children;

    Element(const std::string &tag, const AttrMap &attrs, const std::vector<Node> &kids)
        : tag_name{tag}, attributes{attrs}, children{kids} {}
};

std::tuple<std::string, std::string> parse_attribute(std::string input);

AttrMap parse_attributes(std::string input);

std::tuple<std::string, AttrMap> open_tag(std::string::iterator &begin, std::string::iterator &end);

std::string close_tag(std::string::iterator &begin, std::string::iterator &end);

std::string parse_text(std::string::iterator &begin, std::string::iterator &end);

Element parse_element(std::string::iterator &begin, std::string::iterator &end);

std::vector<Node> nodes(std::string::iterator &begin, std::string::iterator &end);

} // namespace html
