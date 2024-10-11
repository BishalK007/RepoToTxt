#ifndef DISPLAY_SELECTED_COMPONENT_HPP
#define DISPLAY_SELECTED_COMPONENT_HPP

#include <ftxui/component/component.hpp>
#include <set>
#include <filesystem>

namespace fs = std::filesystem;

class DisplaySelectedComponent {
public:
    DisplaySelectedComponent(std::set<fs::path>& selected_paths);
    ftxui::Component Render();

private:
    std::set<fs::path>& selected_paths;
    ftxui::Component display_selected;
};

#endif // DISPLAY_SELECTED_COMPONENT_HPP
