#include "ui/ui_component.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/animation.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;
using namespace ftxui;

UIComponent::UIComponent() : screen(ScreenInteractive::Fullscreen()) {
    current_directory = fs::current_path();
    menu_container = Container::Vertical({}, &focused_index);
}

void UIComponent::BuildMenu() {
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

void UIComponent::Run() {
    // Initialize components
    BuildMenu();

    // Create the "Done" button that prints selected items
    auto done_button = Button("Done", [&] {
        std::cout << "Selected items:" << std::endl;
        for (const auto& path : selected_paths) {
            std::cout << path.string() << std::endl;
        }
        screen.ExitLoopClosure()();
    });

    // Create the "Exit" button
    auto exit_button = Button("Exit", [&] {
        selected_paths.clear(); // Clear selections
        screen.ExitLoopClosure()();
    });

    // Renderer for displaying selected items
    auto display_selected = Renderer([&] {
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

    // Create the instructions element
    auto instructions = Renderer([&] {
        return vbox({
            separator(),
            text("Instructions:") | bold,
            text("↑/↓ Arrow Keys: Navigate"),
            text("Enter: Select/Deselect"),
            text("O/o/Spacebar: Enter/Exit Directory"),
            text("Esc: Jump to 'Done' Button")
        }) | border;
    });

    int left_size = Screen::Create(Dimension::Full()).dimx() / 3;

    // Left component: Add the menu and the buttons (Done and Exit) in a vertical box layout
    auto button_container = Container::Horizontal({
        done_button,
        exit_button,
    });

    auto left_component = Container::Vertical({
        menu_container,
        button_container,
        instructions // Add instructions below the buttons
    });

    auto left_renderer = Renderer(left_component, [&] {
        return vbox({
            text("Current Directory: " + current_directory.string()) | bold | hcenter,
            separator(),
            menu_container->Render() | vscroll_indicator | frame | flex,
            separator(),
            hbox({
                done_button->Render() | size(HEIGHT, EQUAL, 3) | flex,
                exit_button->Render() | size(HEIGHT, EQUAL, 3) | flex,
            }) | hcenter,
            instructions->Render(), // Render the instructions below the buttons
        });
    });

    // Right Component: Display selected items
    auto right_component = Renderer(display_selected, [&] {
        return vbox({
            text("Selected Items") | bold | hcenter,
            separator(),
            display_selected->Render() | flex,
        }) | border;
    });

    // Create a container with resizable left and right components
    auto main_container = ResizableSplitLeft(left_renderer, right_component, &left_size);

    auto main_container_renderer = Renderer(main_container, [&] {
        return main_container->Render() |
               size(WIDTH, EQUAL, Screen::Create(Dimension::Full()).dimx()) |
               size(HEIGHT, EQUAL, Screen::Create(Dimension::Full()).dimy());
    });

    // Event handling with circular navigation for the checkboxes
    auto main_container_with_events = CatchEvent(main_container_renderer, [this, done_button](Event e) -> bool {
        if (e.is_character()) {
            char ch = e.character()[0];
            if (ch == 'q' || ch == 'Q') {
                screen.ExitLoopClosure()();
                return true;
            } else if (ch == 'o' || ch == 'O' || ch == ' ') {
                if (focused_index >= 0 && focused_index < options.size()) {
                    fs::path selected_path = current_directory / options[focused_index];
                    if (options[focused_index] == "..") {
                        current_directory = current_directory.parent_path();
                        BuildMenu();
                        return true;
                    } else if (fs::is_directory(selected_path)) {
                        current_directory = selected_path;
                        BuildMenu();
                        return true;
                    }
                }
            }
        }

        // Implement circular navigation
        if (e == Event::ArrowDown) {
            focused_index = (focused_index + 1) % options.size();
            return true;
        } else if (e == Event::ArrowUp) {
            focused_index = (focused_index == 0) ? options.size() - 1 : focused_index - 1;
            return true;
        }

        if(e == Event::Escape) {
            // Focus on Done button when Escape is pressed
            if(menu_container->Focused()) {
                done_button->TakeFocus();
            } else {
                menu_container->TakeFocus();
            }
            return true;
        }
        return false;
    });

    // Loop the screen for interaction
    screen.Loop(main_container_with_events);

    // After exiting the loop, print selected items (if any)
    std::cout << "Selected items after exit:" << std::endl;
    for (const auto& path : selected_paths) {
        std::cout << path.string() << std::endl;
    }
}
