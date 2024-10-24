#include "ui/menu_component.hpp"

#include <algorithm>
#include <filesystem>
#include <ftxui/component/animation.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "utils/utils.hpp"

using namespace ftxui;

MenuComponent::MenuComponent(
    int& focused_index,
    fs::path& current_directory,
    std::vector<std::string>& options,
    std::vector<std::unique_ptr<bool>>& checkbox_states,
    std::set<fs::path>& selected_paths)
    : focused_index(focused_index),
      current_directory(current_directory),
      options(options),
      checkbox_states(checkbox_states),
      selected_paths(selected_paths),
      menu_container(Container::Vertical({}, &focused_index)) {
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
    options.push_back("..");  // Option to move up one directory
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
                // **Selection Logic: Adding an Item**

                // **1. Identify and Remove Any Parent Directories from selected_paths**
                std::vector<fs::path> parents_to_remove;
                for (const auto& selected_path : selected_paths) {
                    if (Utils::IsParentPath(selected_path, item_path)) {
                        parents_to_remove.emplace_back(selected_path);
                    }
                }
                for (const auto& parent_path : parents_to_remove) {
                    selected_paths.erase(parent_path);

                    // Find the index of the parent in options
                    auto it = std::find_if(options.begin(), options.end(),
                                           [&](const std::string& option) {
                                               return (current_directory / option) == parent_path;
                                           });
                    if (it != options.end()) {
                        size_t parent_index = std::distance(options.begin(), it);
                        if (parent_index < checkbox_states.size()) {
                            // Uncheck the parent checkbox
                            *checkbox_states[parent_index] = false;
                        }
                    }
                }

                // **2. Add the Child Directory to selected_paths**
                selected_paths.insert(item_path);

            } else {
                // **Deselection Logic: Removing an Item**

                // Remove the item_path from selected_paths
                selected_paths.erase(item_path);

                // Optionally, remove any children of the deselected directory
                std::vector<fs::path> paths_to_remove;
                for (const auto& selected_path : selected_paths) {
                    if (Utils::IsParentPath(item_path, selected_path)) {
                        paths_to_remove.emplace_back(selected_path);
                    }
                }
                for (const auto& path : paths_to_remove) {
                    selected_paths.erase(path);

                    // Find the index of the child in options
                    auto it = std::find_if(options.begin(), options.end(),
                                           [&](const std::string& option) {
                                               return (current_directory / option) == path;
                                           });
                    if (it != options.end()) {
                        size_t child_index = std::distance(options.begin(), it);
                        if (child_index < checkbox_states.size()) {
                            // Uncheck the child checkbox
                            *checkbox_states[child_index] = false;
                        }
                    }
                }
            }
        };

        // Add the Checkbox to the container with the CheckboxOption
        auto checkbox = Checkbox(
            options[i] + (is_directory ? "/" : ""),
            checkbox_states[i].get(),
            checkbox_option);
        menu_container->Add(checkbox);
    }
}

ftxui::Component MenuComponent::GetMenuContainer() {
    return menu_container;
}
