#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Element;

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



std::tuple<std::string, std::string> parse_attribute(std::string input);

AttrMap attributes(std::string input);
