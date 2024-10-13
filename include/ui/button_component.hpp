#ifndef BUTTON_COMPONENT_HPP
#define BUTTON_COMPONENT_HPP

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp> // Add this line
#include <set>
#include <filesystem>

namespace fs = std::filesystem;

class ButtonComponent {
public:
    ButtonComponent(ftxui::ScreenInteractive& screen, std::set<fs::path>& selected_paths, std::string& pressedButton);
    ftxui::Component GetCopyAllButton();
    ftxui::Component GetCatAllButton();
    ftxui::Component GetCopyTreeButton();
    ftxui::Component GetCatTreeButton();
    ftxui::Component GetExitButton();

private:
    ftxui::Component copy_all_button;
    ftxui::Component cat_all_button;
    ftxui::Component copy_tree_button;
    ftxui::Component cat_tree_button;
    ftxui::Component exit_button;
    ftxui::ScreenInteractive& screen;
    std::set<fs::path>& selected_paths;
};

#endif // BUTTON_COMPONENT_HPP
