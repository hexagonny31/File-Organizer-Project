#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <functional>

using namespace std::string_literals;
namespace py = pybind11;
namespace fs = std::filesystem;

std::function<void(const std::string&)> py_log_info;
std::function<void(const std::string&)> py_log_error;
std::function<void(const std::string&)> py_log_warn;

void logInfo(const std::string &m) {
    if(py_log_info) py_log_info(m);
    else std::cout << "[INFO] " << m << std::endl;
}

void logError(const std::string &m) {
    if(py_log_error) py_log_error(m);
    else std::cerr << "[ERROR] " << m << std::endl;
}

void logWarning(const std::string &m) {
    if(py_log_warn) py_log_warn(m);
    else std::cout << "[WARNING] " << m << std::endl;
}

std::unordered_map<std::string, fs::path> buildDestMap(const fs::path & src, const std::unordered_map<std::string, std::string> keys) {
    std::unordered_map<std::string, fs::path> dest_map;
    for(const auto &[extension, folder] : keys) {
        fs::path temp = src / folder;
        dest_map[extension] = temp;
        if(dest_map.find(extension) != dest_map.end()) logInfo("Mapped: "s + extension + " to "s + temp.string());
        else logError("Failed to map: "s + extension);
    }
    return dest_map;
}

void moveFile(const fs::path &entry, fs::path destination) {
    destination = destination / entry.filename();
    try {
        rename(entry, destination);
        logInfo("Moved: "s + destination.string());
    } catch(const fs::filesystem_error &e) {
        logError("Failed to move "s + entry.string() + ": "s + e.what());
    }
}

void toSrcDir(const fs::path &src, const std::vector<fs::path> &initDir, const std::unordered_map<std::string, fs::path> &destMap) {
    for(const auto& init : initDir) {
        if(!exists(init) || !is_directory(init)) continue;
        for(const auto &entry : fs::directory_iterator(init)) {
            if(!entry.is_regular_file()) continue;
            const auto &ext = entry.path().extension().string();
            if(destMap.find(ext) != destMap.end()) moveFile(entry.path(), src);
        }
    }
}

void byExt(const fs::path &src, const std::unordered_map<std::string, fs::path> &destMap) {
    try {
        for(const auto& entry : fs::directory_iterator(src)) {
            if(!entry.is_regular_file()) continue;
            const auto &ext = entry.path().extension().string();
            if(destMap.find(ext) != destMap.end()) {
                fs::path destDir = destMap.at(ext);
                if(!exists(destDir)) {
                    try {
                        create_directory(destDir);
                        logInfo("Created folder: "s + destDir.string());
                    } catch(const fs::filesystem_error &e) {
                        logError("Failed to create folder: "s + e.what());
                        continue;
                    }
                }
                moveFile(entry.path(), destDir);
            } else {
                logWarning("Skipped (unmapped extention): "s + entry.path().string());
            }
        }
    } catch(const fs::filesystem_error &e) {
        logError(e.what());
    }
}

void byAlph(const fs::path &src) {
    try {
        for(const auto& entry : fs::directory_iterator(src)) {
            if(!entry.is_regular_file()) continue;
            const auto &filename = entry.path().filename().string();
            char firstChar = std::tolower(filename[0]);
            if(std::isalpha(firstChar)) {
                fs::path destDir = src / std::string(1, firstChar);
                if(!exists(destDir)) {
                    try {
                        create_directory(destDir);
                        logInfo("Created folder: "s + destDir.string());
                    } catch(const fs::filesystem_error &e) {
                        logError("Failed to create folder: "s + e.what());
                        continue;
                    }
                }
                moveFile(entry.path(), destDir);
            } else {
                logWarning("Skipped (non-alphabetic start): "s + entry.path().string());
            }
        }
    } catch(const fs::filesystem_error &e) {
        logError(e.what());
    }
}

void removeFolder(const fs::path &destination) {
    try {
        fs::remove_all(destination);
        logInfo("Unsorted: "s + destination.string());
    } catch(const fs::filesystem_error &e) {
        logInfo("Deleting folder: "s + e.what());
    }
}

void removeExt(const fs::path& src, const std::unordered_map<std::string, fs::path> &destMap) {
    try {
        for(const auto &entry : destMap) {
            const fs::path &destDir = entry.second;
            if(!exists(destDir) || !is_directory(destDir)) {
                logWarning("Directory does not exists or is not a directory: "s + destDir.string());
                continue;
            }
            for(const auto &entry : fs::directory_iterator(destDir)) {
                if(!entry.is_regular_file()) continue;
                moveFile(entry.path(), src);
            }
            removeFolder(destDir);
        }
    } catch(const fs::filesystem_error &e) {
        logError(e.what());
    }
}

void removeAlph(const fs::path &src) {
    try {
        for(const auto &entry : fs::directory_iterator(src)) {
            const fs::path &destDir = entry.path();
            if(!is_directory(destDir)) {
                logWarning("Directory does not exists or is not a directory: "s + destDir.string());
                continue;
            }
            std::string destName = destDir.filename().string();
            if(destName.size() == 1 && std::isupper(destName[0])) {
                for(const auto &entry : fs::directory_iterator(destDir)) {
                    if(!entry.is_regular_file()) continue;
                    moveFile(entry.path(), src);
                }
                removeFolder(destDir);
            }
        }
    } catch(const fs::filesystem_error &e) {
        logError(e.what());
    }
}

PYBIND11_MODULE(file_sorter, m) {
    m.doc() = "A module for sorting files by extension or alphabetically";
    m.def("move_file", &moveFile,
          "Move a file to a destination directory",
          py::arg("entry"), py::arg("destination"));
    m.def("to_src_dir", &toSrcDir,
          "Move files to a targeted source directory",
          py::arg("src"), py::arg("init_dir"), py::arg("dest_map"));
    m.def("by_ext", &byExt,
          "Sort files by extension",
          py::arg("src"), py::arg("dest_map"));
    m.def("by_alph", &byAlph,
          "Sort files alphabetically",
          py::arg("src"));
    m.def("remove_folder", &removeFolder,
          "Remove a folder and all its contents",
          py::arg("destination"));
    m.def("remove_ext", &removeExt,
          "Remove files by extension",
          py::arg("src"), py::arg("dest_map"));
    m.def("remove_alph", &removeAlph,
          "Remove files alphabetically",
          py::arg("src"));
    m.def("build_dest_map", &buildDestMap,
          "Build a mapping of file extensions to destination directories",
          py::arg("src"), py::arg("keys"));

    m.def("set_log_callbacks", [](py::function info, py::function error, py::function warn) {
        py_log_info  = [info] (const std::string& s) { info(s); };
        py_log_error = [error](const std::string& s) { error(s); };
        py_log_warn  = [warn] (const std::string& s) { warn(s); };
    });
}