#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <string>
#include "./htmlParser.hpp"


namespace qi = boost::spirit::qi;

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


