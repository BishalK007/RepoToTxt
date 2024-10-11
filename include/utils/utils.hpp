// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <vector>
#include <string>

namespace Utils {
    /**
     * @brief Recursively prints the directory tree of the selected paths starting from the root.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @param root The root path from which to start printing the tree.
     */
    void PrintDirectoryTree(const std::vector<std::filesystem::path>& selected_paths, const std::filesystem::path& root);
    
    /**
     * @brief Recursively prints the contents of the selected files to stdout.
     *        If a directory is selected, it traverses all its subdirectories and prints the contents of all regular files.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     */
    void PrintFileContents(const std::vector<std::filesystem::path>& selected_paths);
}

#endif // UTILS_H

