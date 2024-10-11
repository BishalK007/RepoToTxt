#include "ui/menu_component.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/animation.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <filesystem>
#include <algorithm>

using namespace ftxui;

MenuComponent::MenuComponent(
    int& focused_index,
    fs::path& current_directory,
    std::vector<std::string>& options,
    std::vector<std::unique_ptr<bool>>& checkbox_states,
    std::set<fs::path>& selected_paths
) : focused_index(focused_index),
    current_directory(current_directory),
    options(options),
    checkbox_states(checkbox_states),
    selected_paths(selected_paths),
    menu_container(Container::Vertical({}, &focused_index))
{
    BuildMenu();
}

void MenuComponent::BuildMenu() {
    // Reset focused_index if needed
    focused_index = 0;

    // Clear previous data
    options.clear();
    checkbox_states.clear();

    // Clear existing children from menu_container
    menu_container->DetachAllChildren();

    // Get directory entries
    options.push_back(".."); // Option to move up one directory
    for (const auto& entry : fs::directory_iterator(current_directory)) {
        options.push_back(entry.path().filename().string());
    }

    // Reserve space for checkbox states
    checkbox_states.reserve(options.size());
    for (size_t i = 0; i < options.size(); ++i) {
        checkbox_states.emplace_back(std::make_unique<bool>(false));
    }

    // Create the menu items
    for (size_t i = 0; i < options.size(); ++i) {
        fs::path item_path = current_directory / options[i];
        *checkbox_states[i] = selected_paths.find(item_path) != selected_paths.end();

        // Determine if the current item is a directory and not ".."
        bool is_directory = fs::is_directory(item_path) && options[i] != "..";

        // Create a CheckboxOption and set the on_change callback
        auto checkbox_option = CheckboxOption::Simple();
        checkbox_option.on_change = [this, i, item_path]() {
            if (options[focused_index] == "..") {
                current_directory = current_directory.parent_path();
                BuildMenu();
                return;
            }
            if (*(checkbox_states[i])) {
                // Check if any parent of item_path exists, if so remove it
                auto parent = item_path.parent_path();
                while (!parent.empty() && parent != parent.root_path()) {
                    if (selected_paths.find(parent) != selected_paths.end()) {
                        selected_paths.erase(parent);
                        break;
                    }
                    parent = parent.parent_path();
                }

                // Check if any children of item_path exist and remove them
                for (auto it = selected_paths.begin(); it != selected_paths.end();) {
                    if (it->string().find(item_path.string()) == 0 && it->string() != item_path.string()) {
                        it = selected_paths.erase(it); // erase child and advance iterator
                    } else {
                        ++it; // advance iterator without erasing
                    }
                }

                // Insert the item_path after all checks
                selected_paths.insert(item_path);

            } else {
                // Uncheck case, remove the item_path
                selected_paths.erase(item_path);
            }
        };

        // Add the Checkbox to the container with the CheckboxOption
        auto checkbox = Checkbox(
            options[i] + (is_directory ? "/" : ""),
            checkbox_states[i].get(),
            checkbox_option
        );
        menu_container->Add(checkbox);
    }
}

ftxui::Component MenuComponent::GetMenuContainer() {
    return menu_container;
}
