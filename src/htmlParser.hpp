#include <string>

struct Attribute {
    std::string attributeName;
    std::string attributeValue;
};

Attribute parse_attribute(std::string input);


