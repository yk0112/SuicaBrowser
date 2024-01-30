#include "layout.hpp"
#include <bit>
#include <iostream>

namespace layout {

LayoutBox::LayoutBox(html::NodeType node,
                     std::string text_data,
                     render::propertyMap properties,
                     render::DisplayType display_type,
                     BoxType box_type) {
    display_type = display_type;
    box_type = box_type;
    properties = properties;

    if (display_type == render::DisplayType::Text) {
        text_node = text_render::constructText(this, text_data);
    }
    // if (box_type == BoxType::Img) {
    // }
}

void LayoutBox::calculateWidth(Dimensions container) {
    int paddingLeft = std::stoi(getStyleValue(constants::css_properties::PADDING_LEFT, "0"));
    int paddingRight = std::stoi(getStyleValue(constants::css_properties::PADDING_RIGHT, "0"));
    int borderLeft = std::stoi(getStyleValue(constants::css_properties::BORDER_LEFT_WIDTH, "0"));
    int borderRight = std::stoi(getStyleValue(constants::css_properties::BORDER_RIGHT_WIDTH, "0"));

    bool margin_left_isAuto = isAuto(constants::css_properties::MARGIN_LEFT);
    int marginLeft;

    bool margin_right_isAuto = isAuto(constants::css_properties::MARGIN_RIGHT);
    int marginRight;

    if (!margin_left_isAuto)
        marginLeft = std::stoi(getStyleValue(constants::css_properties::MARGIN_LEFT, "0"));
    if (!margin_right_isAuto)
        marginRight = std::stoi(getStyleValue(constants::css_properties::MARGIN_RIGHT, "0"));

    int width;
    bool width_isAuto = isAuto(constants::css_properties::WIDTH);

    if (display_type == render::DisplayType::Text) {
        width = text_render::getTextWidth(this);
    } else {
        width = std::stoi(getStyleValue(constants::css_properties::WIDTH, "-1"));
    }

    int total = paddingLeft + paddingRight + std::max(marginLeft, 0) + std::max(marginRight, 0) +
                borderLeft + borderRight + std::max(width, 0);

    if (width != -1 && total > container.content.width) {
        if (margin_left_isAuto) {
            marginLeft = 0;
        }
        if (margin_right_isAuto) {
            marginRight = 0;
        }
    }

    int underflow = container.content.width - total;

    if (this->display_type == render::DisplayType::Text ||
        this->display_type == render::DisplayType::Inline ||
        (this->display_type == render::DisplayType::FlexChild && width_isAuto)) {
        width = total;
    }

    if (width_isAuto) {
        if (margin_left_isAuto) {
            marginLeft = 0;
        }
        if (margin_right_isAuto) {
            marginRight = 0;
        }
        if (underflow > 0) {
            width = underflow;
        } else {
            // Width can't be negative. Adjust the right margin instead.
            width = 0;
            marginRight = marginRight + underflow;
        }
    } else if (margin_right_isAuto && margin_left_isAuto) {
        marginRight = underflow / 2.0;
        marginLeft = underflow / 2.0;
    } else if (margin_left_isAuto) {
        marginLeft = underflow;
    } else if (margin_right_isAuto) {
        marginRight = underflow;
    } else {
        marginRight = marginRight + underflow;
    }

    dimensions.padding.left = paddingLeft;
    dimensions.padding.right = paddingRight;
    dimensions.margin.left = marginLeft;
    dimensions.margin.right = marginRight;
    dimensions.border.left = borderLeft;
    dimensions.border.right = borderRight;
    dimensions.content.width = width;
}

void LayoutBox::calculatePosition(Dimensions container,
                                  int xCursor,
                                  int yCursor,
                                  bool shouldRenderBelow) {
    int paddingTop = std::stoi(getStyleValue(constants::css_properties::PADDING_TOP, "0"));
    int paddingBottom = std::stoi(getStyleValue(constants::css_properties::PADDING_BOTTOM, "0"));
    int borderTop = std::stoi(getStyleValue(constants::css_properties::BORDER_TOP_WIDTH, "0"));
    int borderBottom =
        std::stoi(getStyleValue(constants::css_properties::BORDER_BOTTOM_WIDTH, "0"));

    bool margin_top_isAuto = isAuto(constants::css_properties::MARGIN_LEFT);
    int marginTop = 0;

    bool margin_bottom_isAuto = isAuto(constants::css_properties::MARGIN_RIGHT);
    int marginBottom = 0;

    if (!margin_top_isAuto)
        marginTop = std::stoi(getStyleValue(constants::css_properties::MARGIN_TOP, "0"));
    if (!margin_bottom_isAuto)
        marginBottom = std::stoi(getStyleValue(constants::css_properties::MARGIN_BOTTOM, "0"));

    dimensions.padding.top = paddingTop;
    dimensions.padding.bottom = paddingBottom;
    dimensions.margin.top = marginTop;
    dimensions.margin.bottom = marginBottom;
    dimensions.border.top = borderTop;
    dimensions.border.bottom = borderBottom;

    int x = container.content.x + xCursor + dimensions.padding.left + dimensions.border.left +
            dimensions.margin.left;
    int y = yCursor + container.content.y + dimensions.padding.top + dimensions.border.top +
            dimensions.margin.top;
    dimensions.content.x = x;
    dimensions.content.y = y;
}

void LayoutBox::layoutChildren(int parentWidth) {
    int availableChildWidth;

    if (display_type == render::DisplayType::Inline ||
        display_type == render::DisplayType::FlexChild) {
        availableChildWidth = parentWidth;
    } else {
        availableChildWidth = dimensions.content.width;
    }

    for (auto &child : children) {}
}

void LayoutBox::applyLayout(Dimensions container,
                            int xCursor,
                            int yCursor,
                            bool shouldRenderBelow) {
    calculateWidth(container);
    calculatePosition(container, xCursor, yCursor, shouldRenderBelow);
    layoutChildren(container.content.width - container.padding.left - container.padding.right);
    setHeight();
}

BoxType ToBoxType(const std::string &tag_name) {
    if (tag_name == "img") {
        return BoxType::Img;
    } else if (tag_name == "text") {
        return BoxType::Text;
    } else if (tag_name == "bullet") {
        return BoxType::Bullet;
    } else {
        return BoxType::Shape;
    }
}

std::unique_ptr<layout::LayoutBox> build_layout_tree(const render::StyledNode &styleTree) {
    std::unique_ptr<layout::LayoutBox> layoutTree(new LayoutBox(styleTree.node_type,
                                                                styleTree.text_data,
                                                                styleTree.properties,
                                                                styleTree.get_display_type(),
                                                                ToBoxType(styleTree.tag_name)));

    for (auto child : styleTree.children) {
        std::unique_ptr<LayoutBox> childTree = build_layout_tree(*child);
        layoutTree->addChild(std::move(childTree));
    }
    return layoutTree;
}

} // namespace layout
