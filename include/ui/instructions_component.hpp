#ifndef INSTRUCTIONS_COMPONENT_HPP
#define INSTRUCTIONS_COMPONENT_HPP

#include <ftxui/component/component.hpp>

class InstructionsComponent {
public:
    InstructionsComponent();
    ftxui::Component Render();

private:
    ftxui::Component instructions;
};

#endif // INSTRUCTIONS_COMPONENT_HPP
