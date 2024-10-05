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

    // Create the "Done" button
    auto done_button = Button("Done", [&] { screen.ExitLoopClosure()(); });

    // Create the "Exit" button
    auto exit_button = Button("Exit", [&] { 
        selected_paths.clear(); // Clear selections
        screen.ExitLoopClosure()(); 
    });

    // Custom renderer to display ticks next to selected items and highlight the focused item
    auto custom_menu = CatchEvent(
        Renderer(menu, [&] {
            Elements menu_elements;
            for (size_t i = 0; i < options.size(); ++i) {
                // Determine if the current item is selected
                fs::path item_path = current_directory / options[i];
                bool is_selected = selected_paths.find(item_path) != selected_paths.end();
                std::string prefix = is_selected ? "✔ " : "  ";
                
                // Append '/' to directories for visual distinction, excluding '..'
                std::string display_name = options[i];
                if (fs::is_directory(item_path) && options[i] != "..") {
                    display_name += "/";
                }

                // Create the line element with prefix and display name
                Element line = text(prefix + display_name);

                // Highlight the currently focused item using background color
                if (static_cast<int>(i) == selected_index) {
                    line = line | bgcolor(Color::Blue) | color(Color::White);
                }

                menu_elements.push_back(line);
            }
            return vbox(menu_elements) | frame | border;
        }),
        [&](Event e) {
            // Handle 'Enter' key to toggle selection
            if (e == Event::Return) { // Enter key
                if (selected_index >= 0 && selected_index < options.size()) {
                    fs::path selected_path = current_directory / options[selected_index];
                    if (selected_path.filename() == "..") {
                        // Do not allow selecting '..'
                        return false;
                    }
                    if (selected_paths.find(selected_path) != selected_paths.end()) {
                        selected_paths.erase(selected_path);
                    } else {
                        selected_paths.insert(selected_path);
                    }
                }
                return true; // Event handled
            }

            // Handle 'o' or 'O' key to open directories
            if (e.is_character()) {
                std::string characters = e.character();
                if (!characters.empty()) {
                    char c = characters[0];
                    if (c == 'o' || c == 'O') {
                        if (selected_index >= 0 && selected_index < options.size()) {
                            fs::path selected_path = current_directory / options[selected_index];
                            if (fs::is_directory(selected_path)) {
                                if (options[selected_index] == "..") {
                                    current_directory = current_directory.parent_path();
                                } else {
                                    current_directory = selected_path;
                                }
                                LoadCurrentDirectory();
                                return true; // Event handled
                            }
                        }
                    }
                }
            }

            // Handle 'Esc' key to shift focus to the "Done" button
            if (e == Event::Escape) { // Esc key
                // Post a Tab event to shift focus to the next focusable component ("Done" button)
                screen.PostEvent(Event::Tab);
                return true; // Event handled
            }

            return false; // Event not handled
        }
    );

    // Renderer to display selected items in real-time with '/' appended to directories
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

    // Arrange the components vertically with proper focus management
    auto container = Container::Vertical({
        custom_menu,
        display_selected,
        done_button,
        exit_button
    });

    // Create a renderer to include the current directory path
    auto layout = Renderer(container, [&] {
        return vbox({
            text("Current Directory: " + current_directory.string()) | bold,
            separator(),
            container->Render()
        });
    });

    // Start the event loop
    screen.Loop(layout);

    // After exiting the loop, print selected items
    std::cout << "Selected items:" << std::endl;
    for (const auto& path : selected_paths) {
        std::cout << path.string() << std::endl;
    }
}
