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

    std::vector<std::string> options;                     // Holds the names of files and folders
    int selected_index;                                    // Currently selected index
    ftxui::ScreenInteractive screen;                       // FTXUI screen for displaying the UI
    std::filesystem::path current_directory;               // Current directory path

    std::set<std::filesystem::path> selected_paths;        // Set of all selected paths
};

#endif // UI_COMPONENT_HPP
