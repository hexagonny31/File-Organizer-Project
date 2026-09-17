#pragma once

#include <filesystem>
#include <unordered_map>
#include <functional>

namespace fs = std::filesystem;

enum class ConflictPolicy {
    Rename,
    Skip,
    Overwrite,
};

extern std::function<void(const std::string&)> py_log_info;
extern std::function<void(const std::string&)> py_log_error;
extern std::function<void(const std::string&)> py_log_warn;
extern std::function<int(const std::string& filename)> py_conflict_decision;

void logInfo(const std::string &m);     // Logs an info message.
void logError(const std::string &m);    // Logs an error message.
void logWarning(const std::string &m);  // Logs a warning message.

/**
 * @brief Builds a mapping of file extensions to destination directories.
 * @param src The source directory path.
 * @param keys A mapping of file extensions to folder names.
 * @param log_print If true, logs the mapping process; defaults to true.
 * @return An unordered_map where keys are file extensions and values are destination paths.
 * @note This function does not create the destination directories; it only builds the mapping.
*/
std::unordered_map<std::string, fs::path> buildDestMap(const fs::path & src,
                                                       const std::unordered_map<std::string, std::string> keys,
                                                       bool log_print = true);

/**
 * @brief Moves a file to a specified destination directory.
 * @param entry The path to the file to be moved.
 * @param folder The destination directory path.
 */
void moveFile(const fs::path &entry,
              fs::path folder);

/**
 * @brief Sorts files in the source directory into their respective directories based on their paths.
 * @param src The source directory path.
 * @param init_dir A vector of initial directory paths.
 * @param dest_map A mapping of file extensions to destination paths.
 */
void toSrcDir(const fs::path &src,
              const std::vector<fs::path> &init_dir,
              const std::unordered_map<std::string,
              fs::path> &dest_map);

/**
 * @brief Sorts files in the source directory by their extensions.
 * @param src The source directory path.
 * @param dest_map A mapping of file extensions to destination paths.
 */
void byExt(const fs::path &src,
           const std::unordered_map<std::string, fs::path> &dest_map);

/**
 * @brief Sorts files in the source directory alphabetically.
 * @param src The source directory path.
 */
void byAlph(const fs::path &src);

/**
 * @brief Removes a folder and all its contents.
 * @param destination The path to the folder to be removed.
 */
void removeFolder(const fs::path &destination);

/**
 * @brief Removes files from the source directory based on their extensions.
 * @param src The source directory path.
 * @param dest_map A mapping of file extensions to destination paths.
 */
void removeExt(const fs::path& src,
               const std::unordered_map<std::string, fs::path> &dest_map);

/**
 * @brief Removes files from the source directory alphabetically.
 * @param src The source directory path.
 */
void removeAlph(const fs::path &src);