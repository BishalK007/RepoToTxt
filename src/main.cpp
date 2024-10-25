#ifdef __INTELLISENSE__
#include "utils/version.hpp.in"  // For IntelliSense
#else
#include "utils/version.hpp"  // For actual builds
#endif

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <string>

#include "ui/ui_component.hpp"

int main(int argc, char* argv[]) {
    // Check if --version or -v is passed
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--version" || arg == "-v") {
            std::cout << "RepoToTxt version " << PROJECT_VERSION << std::endl;
            return 0;
        }
    }

    // Proceed with the UI if no version flag is detected
    UIComponent ui;
    ui.Run();
    return 0;
}
