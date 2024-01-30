#pragma once

#include "../layout/text.h"
#include "../render/render.hpp"
#include <QLabel>
#include <iostream>
#include <string>
#include <vector>

namespace layout {

struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

struct EdgeSizes {
    int left = 0;
    int right = 0;
    int top = 0;
    int bottom = 0;
};

// box model of css
struct Dimensions {
    // Position of the content area relative to the document origin
    Rect content;

    // Surrounding edges
    EdgeSizes padding;
    EdgeSizes border;
    EdgeSizes margin;
};

enum class BoxType { Img, Text, Bullet, Shape };

BoxType ToBoxType(const std::string &tag_name);

struct LayoutBox {

    LayoutBox(html::NodeType node,
              std::string text_data,
              render::propertyMap properties,
              render::DisplayType display_type,
              BoxType box_type);

    void calculateWidth(Dimensions container);

    void calculatePosition(Dimensions container, int xCursor, int yCursor, bool shouldRenderBelow);

    void applyLayout(Dimensions container,
                     int xCursor = 0,
                     int yCursor = 0,
                     bool shouldRenderBelow = true);

    void layoutChildren(int parentWidth);

    void addChild(std::unique_ptr<LayoutBox> child) {
        children.push_back(std::move(child));
    }

    std::string getStyleValue(const std::string &property, const std::string defaultValue) const {
        return render::get_property_value(properties, property, defaultValue);
    }

    bool isAuto(const std::string &property) {
        if (properties.find(property) != properties.end() && properties.at(property) == "auto")
            return true;
        return false;
    }

    const QLabel &get_text_node() {
        return *text_node;
    }

  private:
    html::NodeType node;
    render::propertyMap properties;
    render::DisplayType display_type;
    std::vector<std::unique_ptr<LayoutBox>> children;
    Dimensions dimensions;
    BoxType box_type;
    std::unique_ptr<QLabel> text_node;
};

std::unique_ptr<LayoutBox> build_layout_tree(const render::StyledNode &styleTree);

std::unique_ptr<LayoutBox> build_layout_tree(const render::StyledNode &styleTree);

} // namespace layout
