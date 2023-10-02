#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;

struct Attribute {
    std::string attributeName;
    std::string attributeValue;
};

Attribute parse_attribute(std::string input) {
    auto begin = input.begin();
    auto end = input.end();
    Attribute output;

    qi::rule<std::string::iterator, std::string()> attributeNameRule =
        qi::lexeme[+(qi::char_ - qi::lit("="))];
    qi::rule<std::string::iterator, std::string()> attributeValueRule =
        qi::lexeme['"' >> +(qi::char_ - '"') >> '"'];

    bool success = qi::phrase_parse(begin,
                                    end,
                                    attributeNameRule >> qi::lit("=") >> attributeValueRule,
                                    qi::space,
                                    output.attributeName,
                                    output.attributeValue);

    if (success && begin == end) {
        return output;
    }

    std::cout << "fail to parse attribute \n";
    exit(0);
}

int main(int argc, char *argv[]) {
    std::string str("test=\"foobar\""); // パース対象の文字列
    Attribute result = parse_attribute(str);
    std::cout << result.attributeName << std::endl;
    std::cout << result.attributeValue << std::endl;
    return 0;
}


