#include "ui/button_component.hpp"
#include <iostream>

using namespace ftxui;

ButtonComponent::ButtonComponent(ftxui::ScreenInteractive& screen, std::set<fs::path>& selected_paths)
    : screen(screen), selected_paths(selected_paths)
{
    copy_all_button = Button("Copy All", [&] {
        std::cout << "Selected items:" << std::endl;
        for (const auto& path : selected_paths) {
            std::cout << path.string() << std::endl;
        }
        screen.ExitLoopClosure()();
    });
    cat_all_button = Button("Cat All", [&] {
        std::cout << "Selected items:" << std::endl;
        for (const auto& path : selected_paths) {
            std::cout << path.string() << std::endl;
        }
        screen.ExitLoopClosure()();
    });
    copy_tree_button = Button("Copy (Tree)", [&] {
        std::cout << "Selected items:" << std::endl;
        for (const auto& path : selected_paths) {
            std::cout << path.string() << std::endl;
        }
        screen.ExitLoopClosure()();
    });
    cat_tree_button = Button("Cat (Tree)", [&] {
        std::cout << "Selected items:" << std::endl;
        for (const auto& path : selected_paths) {
            std::cout << path.string() << std::endl;
        }
        screen.ExitLoopClosure()();
    });

    exit_button = Button("Exit", [&] {
        selected_paths.clear(); // Clear selections
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
