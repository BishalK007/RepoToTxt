#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

#include <vector>
#include <string>
#include <iostream>
#include <set>
#include <filesystem>
#include <memory>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

#include "ui/menu_component.hpp"
#include "ui/instructions_component.hpp"
#include "ui/display_selected_component.hpp"
#include "ui/button_component.hpp"

class UIComponent {
public:
    UIComponent();
    void Run();

private:
    int selected_index = 0;                        // Currently selected index
    int focused_index = 0;                         // Selector variable for focused item
    ftxui::ScreenInteractive screen;               // FTXUI screen for displaying the UI
    std::filesystem::path current_directory;       // Current directory path
    const std::filesystem::path root_path;         // Fixed root path for relative calculations

    std::vector<std::string> options;              // Holds the names of files and folders
    std::vector<std::unique_ptr<bool>> checkbox_states;    // Checkbox states
    std::set<std::filesystem::path> selected_paths;        // Set of all selected paths

    MenuComponent menu_component;
    InstructionsComponent instructions_component;
    DisplaySelectedComponent display_selected_component;
    ButtonComponent button_component;
};

#endif // UI_COMPONENT_HPP
