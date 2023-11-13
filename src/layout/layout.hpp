#include "../css/cssParser.hpp"
#include "../html/htmlParser.hpp"

namespace layout {

bool css_matches(const html::Node &node, const css::CSSRule &rule);

bool selector_matches(const html::Node &n, const css::Selector &s);

bool AttributeSelector_matches(const html::Element &e, const css::Selector &s);

} // namespace layout
