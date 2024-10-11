#include "ui/ui_component.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <iostream>

using namespace ftxui;

UIComponent::UIComponent()
    : screen(ScreenInteractive::Fullscreen()),
      current_directory(std::filesystem::current_path()),
      menu_component(focused_index, current_directory, options, checkbox_states, selected_paths),
      instructions_component(),
      display_selected_component(selected_paths),
      button_component(screen, selected_paths)
{
}

void UIComponent::Run() {
    // Initialize components
    menu_component.BuildMenu();

    // Get the components
    auto menu_container = menu_component.GetMenuContainer();
    auto instructions = instructions_component.Render();
    auto display_selected = display_selected_component.Render();
    auto done_button = button_component.GetDoneButton();
    auto exit_button = button_component.GetExitButton();

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

    int left_size = Screen::Create(Dimension::Full()).dimx() / 3;

    // Create a container with resizable left and right components
    auto main_container = ResizableSplitLeft(left_renderer, right_component, &left_size);

    auto main_container_renderer = Renderer(main_container, [&] {
        return main_container->Render() |
               size(WIDTH, EQUAL, Screen::Create(Dimension::Full()).dimx()) |
               size(HEIGHT, EQUAL, Screen::Create(Dimension::Full()).dimy());
    });

    // Event handling with circular navigation for the checkboxes
    auto main_container_with_events = CatchEvent(main_container_renderer, [this, menu_container, done_button](Event e) -> bool {
        if (e.is_character()) {
            char ch = e.character()[0];
            if (ch == 'q' || ch == 'Q') {
                screen.ExitLoopClosure()();
                return true;
            } else if (ch == 'o' || ch == 'O' || ch == ' ') {
                if (focused_index >= 0 && focused_index < options.size()) {
                    std::filesystem::path selected_path = current_directory / options[focused_index];
                    if (options[focused_index] == "..") {
                        current_directory = current_directory.parent_path();
                        menu_component.BuildMenu();
                        return true;
                    } else if (std::filesystem::is_directory(selected_path)) {
                        current_directory = selected_path;
                        menu_component.BuildMenu();
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
