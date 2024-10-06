#include "ui/ui_component.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include <set>

namespace fs = std::filesystem;
using namespace ftxui;

UIComponent::UIComponent() 
    : screen(ScreenInteractive::TerminalOutput()), 
      selected_index(0), 
      current_directory(fs::current_path())
{
    LoadCurrentDirectory();
}

void UIComponent::LoadCurrentDirectory() {
    options.clear();

    // Add parent directory entry if not at root
    if (current_directory.has_parent_path()) {
        options.push_back("..");
    }

    try {
        for (const auto& entry : fs::directory_iterator(current_directory)) {
            options.push_back(entry.path().filename().string());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }

    selected_index = 0; // Reset selection to the first item
}

void UIComponent::Run() {
    // Create the menu component
    auto menu = Menu(&options, &selected_index);

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
            text("Selected Items:") | bold,
            separator(),
            selected_items.empty() ? text("None") : vbox(selected_items)
        }) | border;
    });

    int left_size = Screen::Create(Dimension::Full()).dimx() / 3;

    //Left contains menu list, action buttons, Instructions ______________________________________
    
    checkbox_states.reserve(options.size());
    for (size_t i = 0; i < options.size(); ++i) {
        checkbox_states.emplace_back(std::make_unique<bool>(false));
    }

    // Create the menu container
    auto menu_container = Container::Vertical({});

    for (size_t i = 0; i < options.size(); ++i) {
        fs::path item_path = current_directory / options[i];
        *checkbox_states[i] = selected_paths.find(item_path) != selected_paths.end();

        // Determine if the current item is a directory and not ".."
        bool is_directory = fs::is_directory(item_path) && options[i] != "..";

        // Create a CheckboxOption and set the on_change callback
        auto checkbox_option = CheckboxOption::Simple();
        checkbox_option.on_change = [this, i, item_path]() {
            if (*(checkbox_states[i])) {
                selected_paths.insert(item_path);
            } else {
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

    auto menu_renderer = Renderer(menu_container, [&] {
        // No need to clear and repopulate selected_paths here
        return menu_container->Render() | vscroll_indicator | frame | xflex_grow |
            size(HEIGHT, EQUAL, Screen::Create(Dimension::Full()).dimy() / 2) | border;
    });

    // Left component: Add the menu and the buttons (Done and Exit) in a vertical box layout
    auto left_component = Renderer(menu_renderer, [&] { 
        return vbox({
            menu_renderer->Render(),
            done_button->Render() | size(HEIGHT, EQUAL, 3) | center ,
            exit_button->Render() | size(HEIGHT, EQUAL, 3) | center,
        });
    });

    // Right Component ___________________________________________________________________
    auto right_component = Renderer(display_selected, [&] { 
        return vflow({
            text("Random number: " + std::to_string(rand() % 100)),
            display_selected->Render() | center
        }); 
    });

    // Create a container with resizable left and right components.
    auto main_container = ResizableSplitLeft(left_component, right_component, &left_size);

    auto main_container_renderer = Renderer(main_container, [&] {
        return main_container->Render() |
               size(WIDTH, EQUAL, Screen::Create(Dimension::Full()).dimx()) |
               size(HEIGHT, EQUAL, Screen::Create(Dimension::Full()).dimy());
    });

    // For all events
    auto main_container_with_events = CatchEvent(main_container_renderer, [&] (Event e) -> bool {
        if (e.is_character() && e.character()[0] == 'q') {
            screen.Exit();
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
    std::cout<<"  "<<selected_index<<"\n";
}

