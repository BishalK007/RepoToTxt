#include "utils/utils.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <cstdio>
#elif __linux__
#include <cstdio>
#endif

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
     * @param out_stream The output stream to write the tree to.
     */
    void PrintDirectoryTreeHelper(const std::filesystem::path& path, const std::string& prefix, bool isLast, std::ostream& out_stream) {
        out_stream << prefix;

        if (isLast) {
            out_stream << "└── ";
        } else {
            out_stream << "├── ";
        }

        out_stream << path.filename().string() << "\n";

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
                PrintDirectoryTreeHelper(entries[i].path(), new_prefix, last, out_stream);
            }
        }
    }

    /**
     * @brief Generates the directory tree of the selected paths starting from the root.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @param root The root path from which to start generating the tree.
     * @param out_stream The output stream to write the tree to.
     */
    void PrintDirectoryTree(const std::vector<std::filesystem::path>& selected_paths, const std::filesystem::path& root, std::ostream& out_stream) {
        // Print the root
        out_stream << root.string() << " (root)\n";

        // Iterate through each selected path
        for (const auto& path : selected_paths) {
            // Only process paths that are subpaths of root
            if (IsSubPath(root, path)) {
                // Calculate the relative path from root
                std::filesystem::path relative = std::filesystem::relative(path, root);

                // If the relative path is empty, it's the root itself
                if (relative.empty()) continue;

                // Build the full path
                std::filesystem::path current = root / relative;

                // Determine if it's the last item in its directory
                bool isLast = true; // For simplicity, assume it's the last

                PrintDirectoryTreeHelper(current, "", isLast, out_stream);
            }
        }
    }

    /**
     * @brief Recursively prints the contents of the selected files to the given output stream.
     *        If a directory is selected, it traverses all its subdirectories and prints the contents of all regular files.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @param out_stream The output stream to write the file contents to.
     */
    void PrintFileContents(const std::vector<std::filesystem::path>& selected_paths, std::ostream& out_stream) {
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
            out_stream << "\nContents of " << file_path.string() << ":\n";

            std::ifstream file(file_path);
            if (file) {
                std::string line;
                while (std::getline(file, line)) {
                    out_stream << line << "\n";
                }
            } else {
                out_stream << "Failed to open " << file_path.string() << "\n";
            }
        }
    }

    /**
     * @brief Gets the contents of the selected files and directories as a single string.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @return A string containing all the file contents concatenated.
     */
    std::string GetFileContents(const std::vector<std::filesystem::path>& selected_paths) {
        std::ostringstream oss;

        // Collect all files to read
        std::vector<std::filesystem::path> files_to_read;

        for (const auto& path : selected_paths) {
            if (std::filesystem::is_regular_file(path)) {
                files_to_read.emplace_back(std::filesystem::absolute(path));
            } else if (std::filesystem::is_directory(path)) {
                // Recursively iterate through directory and collect all regular files
                for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                    if (std::filesystem::is_regular_file(entry.path())) {
                        files_to_read.emplace_back(std::filesystem::absolute(entry.path()));
                    }
                }
            }
        }

        // Remove duplicate files
        std::sort(files_to_read.begin(), files_to_read.end());
        files_to_read.erase(std::unique(files_to_read.begin(), files_to_read.end()), files_to_read.end());

        for (const auto& file_path : files_to_read) {
            oss << "\nContents of " << file_path.string() << ":\n";

            std::ifstream file(file_path);
            if (file) {
                std::string line;
                while (std::getline(file, line)) {
                    oss << line << "\n";
                }
            } else {
                oss << "Failed to open " << file_path.string() << "\n";
            }
        }
        return oss.str();
    }

    bool IsParentPath(const std::filesystem::path& potential_parent, const std::filesystem::path& potential_child) {
        // Attempt to compute the relative path from potential_parent to potential_child
        std::error_code ec;
        std::filesystem::path relative = std::filesystem::relative(potential_child, potential_parent, ec);
        if (ec) {
            return false;
        }
        // If the relative path starts with "..", then potential_parent is not a parent
        return !relative.empty() && relative.string().find("..") != 0;
    }

    bool IsChildPath(const std::filesystem::path& potential_parent, const std::filesystem::path& potential_child) {
        // A child path is when potential_parent is a parent of potential_child
        return IsParentPath(potential_parent, potential_child);
    }

    /**
     * @brief Copies the given text to the system clipboard.
     * 
     * @param text The text to copy to the clipboard.
     * @return true If the operation was successful.
     * @return false Otherwise.
     */
    bool CopyToClipboard(const std::string& text) {
    #ifdef _WIN32
        // Windows clipboard implementation
        if (!OpenClipboard(nullptr)) {
            return false;
        }
        EmptyClipboard();

        HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, text.size() + 1);
        if (!hGlob) {
            CloseClipboard();
            return false;
        }
        memcpy(hGlob, text.c_str(), text.size() + 1);
        SetClipboardData(CF_TEXT, hGlob);
        CloseClipboard();
        GlobalFree(hGlob);
        return true;

    #elif __APPLE__
        // macOS implementation using pbcopy
        FILE* pipe = popen("pbcopy", "w");
        if (pipe == nullptr) {
            return false;
        }
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        pclose(pipe);
        return true;

    #elif __linux__
        // Linux implementation using xclip
        FILE* pipe = popen("xclip -selection clipboard", "w");
        if (pipe == nullptr) {
            return false;
        }
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        pclose(pipe);
        return true;

    #else
        // Other platforms
        return false;
    #endif
    }

} // namespace Utils
