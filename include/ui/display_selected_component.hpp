#ifndef DISPLAY_SELECTED_COMPONENT_HPP
#define DISPLAY_SELECTED_COMPONENT_HPP

#include <ftxui/component/component.hpp>
#include <set>
#include <filesystem>
#include <map>
#include <string>

namespace fs = std::filesystem;

class DisplaySelectedComponent {
public:
    DisplaySelectedComponent(std::set<fs::path>& selected_paths, const fs::path& root_path);
    ftxui::Component Render();

private:
    std::set<fs::path>& selected_paths;
    const fs::path root_path;
    ftxui::Component display_selected;

    // Helper structures and functions for building the tree
    struct TreeNode {
        std::string name;
        bool is_directory = false;
        std::map<std::string, TreeNode> children;
    };

    void BuildTree(TreeNode& node);
    void InsertPath(TreeNode& node, const fs::path& relative_path, const fs::path& full_path);
    ftxui::Element RenderTree(const TreeNode& node, int depth = 0);
};

#endif // DISPLAY_SELECTED_COMPONENT_HPP
