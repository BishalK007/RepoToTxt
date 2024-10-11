#include "ui/instructions_component.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

InstructionsComponent::InstructionsComponent() {
    instructions = Renderer([&] {
        return vbox({
            separator(),
            text("Instructions:") | bold,
            text("↑/↓ Arrow Keys: Navigate"),
            text("Enter: Select/Deselect"),
            text("O/o/Spacebar: Enter/Exit Directory"),
            text("Esc: Jump to 'Done' Button")
        }) | border;
    });
}

ftxui::Component InstructionsComponent::Render() {
    return instructions;
}
