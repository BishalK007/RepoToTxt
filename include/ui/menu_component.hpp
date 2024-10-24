#ifndef MENU_COMPONENT_HPP
#define MENU_COMPONENT_HPP

#include <filesystem>
#include <ftxui/component/component.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class MenuComponent {
   public:
    MenuComponent(
        int& focused_index,
        fs::path& current_directory,
        std::vector<std::string>& options,
        std::vector<std::unique_ptr<bool>>& checkbox_states,
        std::set<fs::path>& selected_paths);

    void BuildMenu();

    ftxui::Component GetMenuContainer();

   private:
    int& focused_index;
    fs::path& current_directory;
    std::vector<std::string>& options;
    std::vector<std::unique_ptr<bool>>& checkbox_states;
    std::set<fs::path>& selected_paths;

    ftxui::Component menu_container;
};

#endif  // MENU_COMPONENT_HPP
