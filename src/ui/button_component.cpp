#include "ui/button_component.hpp"

#include <iostream>

using namespace ftxui;

ButtonComponent::ButtonComponent(ftxui::ScreenInteractive& screen, std::set<fs::path>& selected_paths, std::string& pressed_button, int& button_focused_index)
    : screen(screen), selected_paths(selected_paths), button_focused_index(button_focused_index) {
    copy_all_button = Button("Copy All", [&] {
        pressed_button = "CoA";
        screen.ExitLoopClosure()();
    });
    cat_all_button = Button("Cat All", [&] {
        pressed_button = "CaA";
        screen.ExitLoopClosure()();
    });
    copy_tree_button = Button("Copy (Tree)", [&] {
        pressed_button = "CoT";
        screen.ExitLoopClosure()();
    });
    cat_tree_button = Button("Cat (Tree)", [&] {
        pressed_button = "CaT";
        screen.ExitLoopClosure()();
    });

    exit_button = Button("Exit", [&] {
        pressed_button = "Ex";
        selected_paths.clear();  // Clear selections
        screen.ExitLoopClosure()();
    });
}

ftxui::Component ButtonComponent::GetCopyAllButton() {
    return copy_all_button;
}
ftxui::Component ButtonComponent::GetCatAllButton() {
    return cat_all_button;
}
ftxui::Component ButtonComponent::GetCopyTreeButton() {
    return copy_tree_button;
}
ftxui::Component ButtonComponent::GetCatTreeButton() {
    return cat_tree_button;
}

ftxui::Component ButtonComponent::GetExitButton() {
    return exit_button;
}
