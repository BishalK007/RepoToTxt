#include "ui/display_selected_component.hpp"

#include <algorithm>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

DisplaySelectedComponent::DisplaySelectedComponent(std::set<fs::path>& selected_paths, const fs::path& root_path)
    : selected_paths(selected_paths), root_path(root_path) {
    display_selected = Renderer([&] {
        // Build the tree structure from selected_paths
        TreeNode root;
        BuildTree(root);

        // Render the tree structure
        auto tree_element = RenderTree(root);

        return vbox({
            selected_paths.empty() ? text("None") : tree_element | flex,
        });
    });
}

ftxui::Component DisplaySelectedComponent::Render() {
    return display_selected;
}

void DisplaySelectedComponent::BuildTree(TreeNode& node) {
    for (const auto& path : selected_paths) {
        fs::path relative_path = fs::relative(path, root_path);
        InsertPath(node, relative_path, path);
    }
}

void DisplaySelectedComponent::InsertPath(TreeNode& node, const fs::path& relative_path, const fs::path& full_path) {
    auto it = relative_path.begin();
    TreeNode* current_node = &node;

    while (it != relative_path.end()) {
        std::string part = it->string();
        auto& child_node = current_node->children[part];
        child_node.name = part;

        // If this is the last component, set is_directory based on the actual path
        if (std::next(it) == relative_path.end()) {
            child_node.is_directory = fs::is_directory(full_path);
        }

        current_node = &child_node;
        ++it;
    }
}

ftxui::Element DisplaySelectedComponent::RenderTree(const TreeNode& node, int depth) {
    std::vector<Element> elements;

    for (const auto& [name, child] : node.children) {
        // Indent based on depth
        auto indent = text(std::string(depth * 2, ' '));
        auto node_name = text(child.name);

        if (child.is_directory || !child.children.empty()) {
            elements.push_back(hbox({indent,
                                     text("📁 ") | color(Color::Yellow),
                                     node_name | bold}));
            elements.push_back(RenderTree(child, depth + 1));
        } else {
            elements.push_back(hbox({indent,
                                     text("📄 ") | color(Color::Green),
                                     node_name}));
        }
    }

    return vbox(std::move(elements));
}
