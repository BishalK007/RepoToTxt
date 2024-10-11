#ifndef BUTTON_COMPONENT_HPP
#define BUTTON_COMPONENT_HPP

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp> // Add this line
#include <set>
#include <filesystem>

namespace fs = std::filesystem;

class ButtonComponent {
public:
    ButtonComponent(ftxui::ScreenInteractive& screen, std::set<fs::path>& selected_paths);
    ftxui::Component GetDoneButton();
    ftxui::Component GetExitButton();

private:
    ftxui::Component done_button;
    ftxui::Component exit_button;
    ftxui::ScreenInteractive& screen;
    std::set<fs::path>& selected_paths;
};

#endif // BUTTON_COMPONENT_HPP
