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
std::function<int(const std::string& filename)> py_conflict_decision;

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

enum class ConflictPolicy {
    Rename,
    Skip,
    Overwrite,
};

std::unordered_map<std::string, fs::path> buildDestMap(const fs::path & src, const std::unordered_map<std::string, std::string> keys, bool log_print = true) {
    std::unordered_map<std::string, fs::path> dest_map;
    for(const auto &[ext, folder] : keys) {
        auto [it, inserted] = dest_map.try_emplace(ext, src / folder);
        if(log_print) {
            if(!inserted) logError("Failed to map: "s + ext);
            else logInfo("Successfully mapped: "s + ext);
        }
    }
    return dest_map;
}

void moveFile(const fs::path &entry, fs::path folder) {
    fs::path destination = folder / entry.filename();
    try {
        if(fs::exists(destination)) {
            ConflictPolicy decision = ConflictPolicy::Rename;
            if(py_conflict_decision) decision = static_cast<ConflictPolicy>(py_conflict_decision(entry.filename().string()));
            switch(decision) {
            case ConflictPolicy::Skip: {
                logInfo("Skipped: "s + entry.filename().string() + " to "s + folder.string());
                return;
            }
            case ConflictPolicy::Rename: {
                fs::path parent = destination.parent_path();
                const std::string stem = destination.stem().string();
                const std::string ext = destination.extension().string();
                int counter = 1;
                fs::path candidate;
                do {
                    candidate = parent / (stem + " (" + std::to_string(counter) + ")" + ext);
                    ++counter;
                } while(fs::exists(candidate));
                destination = candidate;
                logInfo("Renamed: "s + entry.filename().string() + " to "s + destination.filename().string());
                break;
            }
            case ConflictPolicy::Overwrite: {
                remove(destination);
                logInfo("Overwriting: "s + entry.filename().string());
                break;
            }
            default:
                logWarning("Invalid decision for conflict resolution. Defaulting to skipping.");
                return;
            }
        }
        rename(entry, destination);
        logInfo("Moved: "s + entry.string() + " to "s + folder.string());
    } catch(const fs::filesystem_error &e) {
        logError("Failed to move "s + entry.string() + ": "s + e.what());
    }
}

void toSrcDir(const fs::path &src, const std::vector<fs::path> &init_dir, const std::unordered_map<std::string, fs::path> &dest_map) {
    for(const auto& init : init_dir) {
        if(!exists(init) || !is_directory(init)) continue;
        for(const auto &entry : fs::directory_iterator(init)) {
            if(!entry.is_regular_file()) continue;
            const auto &ext = entry.path().extension().string();
            if(dest_map.find(ext) != dest_map.end()) moveFile(entry.path(), src);
        }
    }
}

void byExt(const fs::path &src, const std::unordered_map<std::string, fs::path> &dest_map) {
    try {
        for(const auto& entry : fs::directory_iterator(src)) {
            if(!entry.is_regular_file()) continue;
            const auto &ext = entry.path().extension().string();
            if(dest_map.find(ext) != dest_map.end()) {
                fs::path dest_dir = dest_map.at(ext);
                if(!exists(dest_dir)) {
                    try {
                        create_directory(dest_dir);
                        logInfo("Created folder: "s + dest_dir.string());
                    } catch(const fs::filesystem_error &e) {
                        logError("Failed to create folder: "s + e.what());
                        continue;
                    }
                }
                moveFile(entry.path(), dest_dir);
            } else {
                logWarning("Skipped (unmapped extension): "s + entry.path().string());
            }
        }
    } catch(const fs::filesystem_error &e) {
        logError("Failed to sort files by extension: "s + e.what());
    }
}

void byAlph(const fs::path &src) {
    try {
        for(const auto& entry : fs::directory_iterator(src)) {
            if(!entry.is_regular_file()) continue;
            const auto &filename = entry.path().filename().string();
            char first_char = std::tolower(filename[0]);
            if(std::isalpha(first_char)) {
                fs::path dest_dir = src / std::string(1, first_char);
                if(!exists(dest_dir)) {
                    try {
                        create_directory(dest_dir);
                        logInfo("Created folder: "s + dest_dir.string());
                    } catch(const fs::filesystem_error &e) {
                        logError("Failed to create folder: "s + e.what());
                        continue;
                    }
                }
                moveFile(entry.path(), dest_dir);
            } else {
                logWarning("Skipped (non-alphabetic start): "s + entry.path().string());
            }
        }
    } catch(const fs::filesystem_error &e) {
        logError("Failed to sort files alphabetically: "s + e.what());
    }
}

void removeFolder(const fs::path &destination) {
    try {
        fs::remove_all(destination);
        logInfo("Unsorted: "s + destination.string());
    } catch(const fs::filesystem_error &e) {
        logError("Failed to remove folder: "s + e.what());
    }
}

void removeExt(const fs::path& src, const std::unordered_map<std::string, fs::path> &dest_map) {
    try {
        for(const auto &entry : dest_map) {
            const fs::path &dest_dir = entry.second;
            if(!exists(dest_dir) || !is_directory(dest_dir)) {
                logWarning("Directory does not exists or is not a directory: "s + dest_dir.string());
                continue;
            }
            for(const auto &entry : fs::directory_iterator(dest_dir)) {
                if(!entry.is_regular_file()) continue;
                moveFile(entry.path(), src);
            }
            if(fs::is_empty(dest_dir)) removeFolder(dest_dir);
            else logWarning("Folder was not empty after moving files: "s + dest_dir.string());
        }
    } catch(const fs::filesystem_error &e) {
        logError("Failed to remove extension folders: "s + e.what());
    }
}

void removeAlph(const fs::path &src) {
    try {
        for(const auto &entry : fs::directory_iterator(src)) {
            const fs::path &dest_dir = entry.path();
            if(!is_directory(dest_dir)) {
                logWarning("Directory does not exists or is not a directory: "s + dest_dir.string());
                continue;
            }
            std::string dest_name = dest_dir.filename().string();
            if(dest_name.size() == 1 && std::isupper(dest_name[0])) {
                for(const auto &entry : fs::directory_iterator(dest_dir)) {
                    if(!entry.is_regular_file()) continue;
                    moveFile(entry.path(), src);
                }
                if(fs::is_empty(dest_dir)) removeFolder(dest_dir);
                else logWarning("Folder was not empty after moving files: "s + dest_dir.string());
            }
        }
    } catch(const fs::filesystem_error &e) {
        logError("Failed to remove alphabetically sorted folders: "s + e.what());
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
          py::arg("src"), py::arg("keys"), py::arg("log_print") = true);

    m.def("set_log_callbacks", [](py::function info, py::function error, py::function warn) {
        py_log_info  = [info] (const std::string& s) { info(s); };
        py_log_error = [error](const std::string& s) { error(s); };
        py_log_warn  = [warn] (const std::string& s) { warn(s); };
    });
    m.def("set_conflict_callback", [](py::function cd) {
        py_conflict_decision = [cd](const std::string& name) -> int {
            return cd(name).cast<int>();
        };
    });
}