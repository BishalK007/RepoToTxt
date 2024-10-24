#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "ui/button_component.hpp"
#include "ui/display_selected_component.hpp"
#include "ui/instructions_component.hpp"
#include "ui/menu_component.hpp"

class UIComponent {
   public:
    UIComponent();
    void Run();

   private:
    int selected_index = 0;        // Currently selected index
    int focused_index = 0;         // Selector variable for focused item
    int button_focused_index = 0;  // Focused index for buttons

    ftxui::ScreenInteractive screen;          // FTXUI screen for displaying the UI
    std::filesystem::path current_directory;  // Current directory path
    const std::filesystem::path root_path;    // Fixed root path for relative calculations

    std::vector<std::string> options;                    // Holds the names of files and folders
    std::vector<std::unique_ptr<bool>> checkbox_states;  // Checkbox states
    std::set<std::filesystem::path> selected_paths;      // Set of all selected paths

    MenuComponent menu_component;
    InstructionsComponent instructions_component;
    DisplaySelectedComponent display_selected_component;
    ButtonComponent button_component;

    std::string pressed_button = "Ex";  // This button tracks the last pressed button. Valid values - CoA, CoT, CaA, CaT, Ex(default)
};

#endif  // UI_COMPONENT_HPP
