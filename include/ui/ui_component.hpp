#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

#include <vector>
#include <string>
#include <iostream>
#include <set>
#include <ftxui/component/screen_interactive.hpp>
#include <filesystem>

class UIComponent {
public:
    UIComponent();
    void Run();

private:
    void LoadCurrentDirectory();
    void BuildMenu();  // Function to rebuild the menu when changing directories

    std::vector<std::string> options;              // Holds the names of files and folders
    int selected_index = 0;                        // Currently selected index
    int focused_index = 0;                         // **Selector variable for focused item**
    ftxui::ScreenInteractive screen;               // FTXUI screen for displaying the UI
    std::filesystem::path current_directory;       // Current directory path

    std::set<std::filesystem::path> selected_paths;        // Set of all selected paths
    std::vector<std::unique_ptr<bool>> checkbox_states;    // Checkbox states
    ftxui::Component menu_container;                       // Menu container component
};

#endif // UI_COMPONENT_HPP
