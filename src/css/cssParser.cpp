#include "cssParser.hpp"

namespace css {

namespace qi = boost::spirit::qi;

using Rule = qi::rule<std::string::iterator, std::string()>;

Rule nameRule = +(qi::char_ - ':');
Rule valueRule = +(qi::char_ - ';');
Rule declarationRule = nameRule >> qi::char_(':') >> valueRule;

Declaration declaration(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::cout << input << "\n";
    std::string name{};
    std::string value{};

    bool sucsess = qi::phrase_parse(begin,
                                    end,
                                    *(qi::char_ - ':' - ' ') >> ':' >> *(qi::char_ - ' '),
                                    qi::space,
                                    name,
                                    value);
    std::cout << name << "\n";
    std::cout << value << "\n";

    if (sucsess && begin == end) {
        return Declaration{name, value};
    }

    throw std::runtime_error("fail to declaration");
}

std::vector<Declaration> declarations(std::string input) {
    auto begin = input.begin();
    auto end = input.end();

    std::vector<std::string> decls{};
    std::vector<Declaration> result{};

    bool sucsess =
        qi::phrase_parse(begin, end, (declarationRule || *(qi::space)) % ';', qi::space, decls);

    if (sucsess && begin == end) {
        for (auto &decl : decls) {
            result.push_back(declaration(decl));
        }
        return result;
    }
    throw std::runtime_error("fail to declarations");
}

} // namespace css
