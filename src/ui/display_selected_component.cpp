#include "ui/display_selected_component.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

DisplaySelectedComponent::DisplaySelectedComponent(std::set<fs::path>& selected_paths)
    : selected_paths(selected_paths)
{
    display_selected = Renderer([&] {
        std::vector<Element> selected_items;
        for (const auto& path : selected_paths) {
            std::string display_name = path.filename().string();
            if (fs::is_directory(path) && path.filename() != "..") {
                display_name += "/";
            }
            selected_items.push_back(text(display_name));
        }

        return vbox({
            selected_items.empty() ? text("None") : vbox(std::move(selected_items))
        });
    });
}

ftxui::Component DisplaySelectedComponent::Render() {
    return display_selected;
}
