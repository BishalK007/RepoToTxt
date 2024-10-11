// utils.cpp
#include "utils/utils.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <algorithm>

namespace Utils {

    /**
     * @brief Helper function to check if path1 is a prefix of path2
     * 
     * @param path1 Potential prefix path.
     * @param path2 Path to check.
     * @return true If path1 is a prefix of path2.
     * @return false Otherwise.
     */
    bool IsSubPath(const std::filesystem::path& path1, const std::filesystem::path& path2) {
        auto it1 = path1.begin();
        auto it2 = path2.begin();
        for (; it1 != path1.end() && it2 != path2.end(); ++it1, ++it2) {
            if (*it1 != *it2) {
                return false;
            }
        }
        return it1 == path1.end();
    }

    /**
     * @brief Recursively prints the directory tree structure.
     * 
     * @param path Current path to process.
     * @param prefix String prefix for formatting the tree.
     * @param isLast Boolean indicating if the current item is the last in its directory.
     */
    void PrintDirectoryTreeHelper(const std::filesystem::path& path, const std::string& prefix, bool isLast) {
        std::cout << prefix;

        if (isLast) {
            std::cout << "└── ";
        } else {
            std::cout << "├── ";
        }

        std::cout << path.filename().string() << "\n";

        std::string new_prefix = prefix + (isLast ? "    " : "│   ");

        if (std::filesystem::is_directory(path)) {
            std::vector<std::filesystem::directory_entry> entries;
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                entries.emplace_back(entry);
            }

            // Sort entries alphabetically, directories first
            std::sort(entries.begin(), entries.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
                if (a.is_directory() && !b.is_directory()) return true;
                if (!a.is_directory() && b.is_directory()) return false;
                return a.path().filename().string() < b.path().filename().string();
            });

            for (size_t i = 0; i < entries.size(); ++i) {
                bool last = (i == entries.size() - 1);
                PrintDirectoryTreeHelper(entries[i].path(), new_prefix, last);
            }
        }
    }

    /**
     * @brief Recursively prints the directory tree of the selected paths starting from the root.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @param root The root path from which to start printing the tree.
     */
    void PrintDirectoryTree(const std::vector<std::filesystem::path>& selected_paths, const std::filesystem::path& root) {
        // Print the root
        std::cout << root.string() << " (root)\n";

        // Iterate through each selected path
        for (const auto& path : selected_paths) {
            // Only process paths that are subpaths of root
            if (IsSubPath(root, path)) {
                // Calculate the relative path from root
                std::filesystem::path relative = std::filesystem::relative(path, root);

                // If the relative path is empty, it's the root itself
                if (relative.empty()) continue;

                // Split the relative path into parts
                std::filesystem::path current = root;
                std::string prefix = "";

                for (auto it = relative.begin(); it != relative.end(); ++it) {
                    current /= *it;
                }

                // Determine if this path should be printed (to avoid duplicates)
                // For simplicity, print each selected path as separate entries
                // The helper function handles recursive directories

                // Check if already printed to avoid redundancy
                // This can be enhanced based on specific requirements

                // Determine if it's the last item in its directory
                bool isLast = true; // For simplicity, assume it's the last

                PrintDirectoryTreeHelper(current, prefix, isLast);
            }
        }
    }

    /**
     * @brief Recursively prints the contents of the selected files to stdout.
     *        If a directory is selected, it traverses all its subdirectories and prints the contents of all regular files.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     */
    void PrintFileContents(const std::vector<std::filesystem::path>& selected_paths) {
        // To avoid printing the same file multiple times if it's selected multiple times via different directories
        std::vector<std::filesystem::path> files_to_print;

        for (const auto& path : selected_paths) {
            if (std::filesystem::is_regular_file(path)) {
                files_to_print.emplace_back(std::filesystem::absolute(path));
            } else if (std::filesystem::is_directory(path)) {
                // Recursively iterate through directory and collect all regular files
                for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                    if (std::filesystem::is_regular_file(entry.path())) {
                        files_to_print.emplace_back(std::filesystem::absolute(entry.path()));
                    }
                }
            }
        }

        // Remove duplicate files
        std::sort(files_to_print.begin(), files_to_print.end());
        files_to_print.erase(std::unique(files_to_print.begin(), files_to_print.end()), files_to_print.end());

        for (const auto& file_path : files_to_print) {
            std::cout << "\nContents of " << file_path.string() << ":\n";

            std::ifstream file(file_path);
            if (file) {
                std::string line;
                while (std::getline(file, line)) {
                    std::cout << line << "\n";
                }
            } else {
                std::cout << "Failed to open " << file_path.string() << "\n";
            }
        }
    }

} // namespace Utils
