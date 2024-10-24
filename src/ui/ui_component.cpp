#include "ui/ui_component.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <map>
#include <vector>

#include "utils/utils.hpp"

using namespace ftxui;

// Helper function to build a tree structure from selected paths
void PrintDirectoryTree(const std::vector<std::filesystem::path> &selected_paths, const std::filesystem::path &root) {
    // Create a map to represent the tree
    std::map<std::string, std::vector<std::string>> tree;

    // Populate the tree map
    for (const auto &path : selected_paths) {
        auto relative_path = std::filesystem::relative(path, root);
        std::string current = root.filename().string();
        for (auto it = relative_path.begin(); it != relative_path.end(); ++it) {
            std::string part = it->string();
            tree[current].push_back(part);
            current += "/" + part;
        }
    }

    // Recursive function to print the tree
    std::function<void(const std::string &, const std::string &)> print_tree;
    print_tree = [&](const std::string &node, const std::string &prefix) {
        const auto &children = tree[node];
        for (size_t i = 0; i < children.size(); ++i) {
            bool is_last = (i == children.size() - 1);
            std::cout << prefix;
            if (is_last) {
                std::cout << "└── ";
            } else {
                std::cout << "├── ";
            }
            std::cout << children[i] << "\n";
            std::string new_prefix = prefix + (is_last ? "    " : "│   ");
            print_tree(node + "/" + children[i], new_prefix);
        }
    };

    // Print the root
    std::cout << root.string() << " (root)\n";
    print_tree(root.filename().string(), "");
}

// Helper function to print file contents
void PrintFileContents(const std::vector<std::filesystem::path> &selected_paths) {
    for (const auto &path : selected_paths) {
        if (std::filesystem::is_regular_file(path)) {
            std::cout << "\nContents of " << path.string() << ":\n";
            std::ifstream file(path);
            if (file) {
                std::cout << file.rdbuf() << "\n";
            } else {
                std::cout << "Failed to open " << path.string() << "\n";
            }
        }
    }
}

UIComponent::UIComponent()
    : screen(ScreenInteractive::Fullscreen()),
      current_directory(std::filesystem::current_path()),
      root_path(current_directory),  // Initialize root_path to initial current_directory
      menu_component(focused_index, current_directory, options, checkbox_states, selected_paths),
      instructions_component(),
      display_selected_component(selected_paths, root_path),  // Pass root_path
      button_component(screen, selected_paths, pressed_button, button_focused_index) {
}

void UIComponent::Run() {
    // Initialize components
    menu_component.BuildMenu();

    // Get the components
    auto menu_container = menu_component.GetMenuContainer();
    auto instructions = instructions_component.Render();
    auto display_selected = display_selected_component.Render();
    auto copy_all_button = button_component.GetCopyAllButton();
    auto cat_all_button = button_component.GetCatAllButton();
    auto copy_tree_button = button_component.GetCopyTreeButton();
    auto cat_tree_button = button_component.GetCatTreeButton();
    auto exit_button = button_component.GetExitButton();

    // Create a vertical container for the main button rows
    auto main_buttons_container = Container::Vertical({
        // First Row: Copy All and Cat All
        Container::Horizontal({
            button_component.GetCopyAllButton(),
            button_component.GetCatAllButton(),
        }),
        // Second Row: Copy Tree and Cat Tree
        Container::Horizontal({
            button_component.GetCopyTreeButton(),
            button_component.GetCatTreeButton(),
        }),
        // Third Row: Exit Button spanning both columns
        button_component.GetExitButton(),
    });

    // Renderer for the flexbox layout of buttons
    auto button_container = Container::Horizontal({copy_all_button,
                                                   cat_all_button,
                                                   copy_tree_button,
                                                   cat_tree_button,
                                                   exit_button},
                                                  &button_focused_index);

    auto left_component = Container::Vertical({menu_container,
                                               button_container,
                                               instructions});

    auto left_renderer = Renderer(left_component, [&] { return vbox({
                                                            text("Current Directory: " + current_directory.string()) | bold | hcenter,
                                                            separator(),
                                                            menu_container->Render() | vscroll_indicator | frame | flex,
                                                            separator(),
                                                            flexbox({
                                                                        // First Row: copy_all_button and cat_all_button
                                                                        flexbox({
                                                                                    copy_all_button->Render() | size(HEIGHT, EQUAL, 3) | flex,
                                                                                    cat_all_button->Render() | size(HEIGHT, EQUAL, 3) | flex,
                                                                                },
                                                                                FlexboxConfig().Set(FlexboxConfig::Direction::Row)),

                                                                        // Second Row: copy_tree_button and cat_tree_button
                                                                        flexbox({
                                                                                    copy_tree_button->Render() | size(HEIGHT, EQUAL, 3) | flex,
                                                                                    cat_tree_button->Render() | size(HEIGHT, EQUAL, 3) | flex,
                                                                                },
                                                                                FlexboxConfig().Set(FlexboxConfig::Direction::Row)),

                                                                        // Third Row: exit_button spanning both columns
                                                                        exit_button->Render() | size(HEIGHT, EQUAL, 3) | flex | flex_grow,
                                                                    },
                                                                    FlexboxConfig().Set(FlexboxConfig::Direction::Column)  // Stack rows vertically
                                                                        .Set(FlexboxConfig::JustifyContent::Center)        // Center vertically
                                                                        .Set(FlexboxConfig::AlignItems::Center))           // Center horizontally
                                                                | hcenter,
                                                            instructions->Render(),
                                                        }); });

    // Right Component: Display selected items
    auto right_component = Renderer(display_selected, [&] { return vbox({
                                                                       text("Selected Items") | bold | hcenter,
                                                                       separator(),
                                                                       display_selected->Render() | flex,
                                                                   }) |
                                                                   border; });

    int left_size = Screen::Create(Dimension::Full()).dimx() / 3;

    // Create a container with resizable left and right components
    auto main_container = ResizableSplitLeft(left_renderer, right_component, &left_size);

    auto main_container_renderer = Renderer(main_container, [&] { return main_container->Render() |
                                                                         size(WIDTH, EQUAL, Screen::Create(Dimension::Full()).dimx()) |
                                                                         size(HEIGHT, EQUAL, Screen::Create(Dimension::Full()).dimy()); });

    // Event handling with circular navigation for the checkboxes
    auto main_container_with_events = CatchEvent(main_container_renderer, [this, menu_container, copy_all_button, cat_all_button, copy_tree_button, cat_tree_button, exit_button, button_container](Event e) -> bool {
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
            if(!menu_container->Focused()) {
                if(copy_all_button->Focused()) button_focused_index = 4;
                else if(cat_all_button->Focused()) button_focused_index = 3;
                else if(copy_tree_button->Focused()) button_focused_index = 4;
                else if(cat_tree_button->Focused()) button_focused_index = 4;
                else if(exit_button->Focused()) button_focused_index = 0;
                return true;
            } 
            focused_index = (focused_index + 1) % options.size();
            return true;
        } else if (e == Event::ArrowUp) {
            if(!menu_container->Focused()) {
                if(copy_all_button->Focused()) button_focused_index = 4;
                else if(cat_all_button->Focused()) button_focused_index = 4;
                else if(copy_tree_button->Focused()) button_focused_index = 0;
                else if(cat_tree_button->Focused()) button_focused_index = 1;
                else if(exit_button->Focused()) button_focused_index = 3;
                return true;
            } 
            focused_index = (focused_index == 0) ? options.size() - 1 : focused_index - 1;
            return true;
        }

        if(e == Event::Escape) {
            // Focus on Copy All button when Escape is pressed
            if(menu_container->Focused()) {
                button_container->TakeFocus();
            } else {
                menu_container->TakeFocus();
            }
            return true;
        }
        return false; });

    // Loop the screen for interaction
    screen.Loop(main_container_with_events);

    // After exiting the loop, display the directory tree and file contents
    if (!selected_paths.empty()) {
        // Ensure all selected paths are absolute
        std::vector<std::filesystem::path> absolute_paths;
        for (const auto &path : selected_paths) {
            absolute_paths.push_back(std::filesystem::absolute(path));
        }

        // Determine the common root path
        std::filesystem::path common_root = absolute_paths[0];
        for (const auto &path : absolute_paths) {
            auto it1 = common_root.begin();
            auto it2 = path.begin();
            std::filesystem::path temp_root;
            while (it1 != common_root.end() && it2 != path.end() && *it1 == *it2) {
                temp_root /= *it1;
                ++it1;
                ++it2;
            }
            common_root = temp_root;
            if (common_root.empty()) {
                break;  // No common root
            }
        }

        if (common_root.empty()) {
            // If there's no common root, use the current directory as the base
            common_root = std::filesystem::current_path();
        }

        // When particular button is pressed
        if (pressed_button == "CaA") {
            // Print the directory tree
            Utils::PrintDirectoryTree(absolute_paths, common_root, std::cout);

            // Print the contents of each selected file
            Utils::PrintFileContents(absolute_paths, std::cout);
        } else if (pressed_button == "CaT") {
            // Print the directory tree
            Utils::PrintDirectoryTree(absolute_paths, common_root, std::cout);
        } else if (pressed_button == "CoA") {
            // create a local string_stream
            std::ostringstream string_stream;

            // Print the directory tree to string_stream
            Utils::PrintDirectoryTree(absolute_paths, common_root, string_stream);

            // Print the contents of each selected file to string_stream
            Utils::PrintFileContents(absolute_paths, string_stream);

            // copy string_stream to clipboard
            Utils::CopyToClipboard(string_stream.str());
        } else if (pressed_button == "CoT") {
            // create a local string_stream
            std::ostringstream string_stream;

            // Print the directory tree to string_stream
            Utils::PrintDirectoryTree(absolute_paths, common_root, string_stream);

            // copy string_stream to clipboard
            Utils::CopyToClipboard(string_stream.str());
        }
    } else {
        std::cout << "No items were selected.\n";
    }
}
