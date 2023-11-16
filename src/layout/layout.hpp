#include "../css/cssParser.hpp"
#include "../html/htmlParser.hpp"

namespace layout {

struct StyleNode {
    html::NodeType node_type;
    std::vector<std::shared_ptr<StyleNode>> children;
    std::unordered_map<std::string, std::string> properties;
};

bool css_matches(const html::Node &node, const css::CSSRule &rule);

bool selector_matches(const html::Node &n, const css::Selector &s);

bool AttributeSelector_matches(const html::Element &e, const css::Selector &s);

std::shared_ptr<StyleNode> to_style_node(const html::Node &node, const css::Stylesheet &stylesheet);

} // namespace layout
