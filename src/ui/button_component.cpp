#include "ui/button_component.hpp"
#include <iostream>

using namespace ftxui;

ButtonComponent::ButtonComponent(ftxui::ScreenInteractive& screen, std::set<fs::path>& selected_paths)
    : screen(screen), selected_paths(selected_paths)
{
    done_button = Button("Done", [&] {
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

ftxui::Component ButtonComponent::GetDoneButton() {
    return done_button;
}

ftxui::Component ButtonComponent::GetExitButton() {
    return exit_button;
}
