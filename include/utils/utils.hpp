#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <vector>
#include <string>
#include <ostream>

namespace Utils {
    /**
     * @brief Recursively prints the directory tree of the selected paths starting from the root.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @param root The root path from which to start printing the tree.
     * @param out_stream The output stream to write the tree to.
     */
    void PrintDirectoryTree(const std::vector<std::filesystem::path>& selected_paths, const std::filesystem::path& root, std::ostream& out_stream);
    
    /**
     * @brief Recursively prints the contents of the selected files to the given output stream.
     *        If a directory is selected, it traverses all its subdirectories and prints the contents of all regular files.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @param out_stream The output stream to write the file contents to.
     */
    void PrintFileContents(const std::vector<std::filesystem::path>& selected_paths, std::ostream& out_stream);

    /**
     * @brief Gets the contents of the selected files and directories as a single string.
     * 
     * @param selected_paths Vector of selected file and directory paths.
     * @return A string containing all the file contents concatenated.
     */
    std::string GetFileContents(const std::vector<std::filesystem::path>& selected_paths);

    /**
     * @brief Determines if potential_parent is a parent of potential_child.
     * 
     * @param potential_parent The potential parent path.
     * @param potential_child The potential child path.
     * @return true If potential_parent is a parent of potential_child.
     * @return false Otherwise.
     */
    bool IsParentPath(const std::filesystem::path& potential_parent, const std::filesystem::path& potential_child);

    /**
     * @brief Determines if potential_child is a child of potential_parent.
     * 
     * @param potential_parent The potential parent path.
     * @param potential_child The potential child path.
     * @return true If potential_child is a child of potential_parent.
     * @return false Otherwise.
     */
    bool IsChildPath(const std::filesystem::path& potential_parent, const std::filesystem::path& potential_child);

    /**
     * @brief Copies the given text to the system clipboard.
     * 
     * @param text The text to copy to the clipboard.
     * @return true If the operation was successful.
     * @return false Otherwise.
     */
    bool CopyToClipboard(const std::string& text);
}

#endif // UTILS_H
