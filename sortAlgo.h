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

void logInfo(const std::string &m);
void logError(const std::string &m);
void logWarning(const std::string &m);

std::unordered_map<std::string, fs::path> buildDestMap(const fs::path & src, const std::unordered_map<std::string, std::string> keys, bool log_print = true);
void moveFile(const fs::path &entry, fs::path folder);
void toSrcDir(const fs::path &src, const std::vector<fs::path> &init_dir, const std::unordered_map<std::string, fs::path> &dest_map);

void byExt(const fs::path &src, const std::unordered_map<std::string, fs::path> &dest_map);
void byAlph(const fs::path &src);

void removeFolder(const fs::path &destination);
void removeExt(const fs::path& src, const std::unordered_map<std::string, fs::path> &dest_map);
void removeAlph(const fs::path &src);