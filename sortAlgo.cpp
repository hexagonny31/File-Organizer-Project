#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include <iostream>
#include <filesystem>
#include <unordered_map>

namespace py = pybind11;
namespace fs = std::filesystem;

std::unordered_map<std::string, fs::path> buildDestMap(const fs::path & src, const std::unordered_map<std::string, std::string> keys) {
    std::unordered_map<std::string, fs::path> dest_map;
    for(const auto &[extension, folder] : keys) {
        fs::path temp = src / folder;
        dest_map[extension] = temp;
        if(dest_map.find(extension) != dest_map.end()) std::cout << "Mapped: " << extension << " to " << temp.string() << std::endl;
        else std::cerr << "Failed to map: " << extension << std::endl;
    }
    return dest_map;
}

void moveFile(const fs::path &entry, fs::path destination) {
    destination = destination / entry.filename();
    try {
        rename(entry, destination);
        std::cout << "Moved: " << destination.string() 
                  << "\nTo: " << entry.string() << std::endl;
    } catch(const fs::filesystem_error &e) {
        std::cerr << "Failed to move " << entry.string() 
                  << ": " << e.what() << std::endl;
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
                        std::cout << "Created: " << destDir.string() << std::endl;
                    } catch(const fs::filesystem_error &e) {
                        std::cerr << "Failed to create folder: " << e.what() << std::endl;
                        continue;
                    }
                }
                moveFile(entry.path(), destDir);
            } else {
                std::cout << "Skipped (unmapped extention): " << entry.path().string() << std::endl;
            }
        }
    } catch(const fs::filesystem_error &e) {
        std::cerr << e.what() << std::endl;
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
                        std::cout << "Created: " << destDir.string() << std::endl;
                    } catch(const fs::filesystem_error &e) {
                        std::cerr << "Failed to create folder: " << e.what() << std::endl;
                        continue;
                    }
                }
                moveFile(entry.path(), destDir);
            } else {
                std::cout << "Skipped (non-alphabetic start): " << entry.path().string() << std::endl;
            }
        }
    } catch(const fs::filesystem_error &e) {
        std::cerr << e.what() << std::endl;
    }
}

void removeFolder(const fs::path &destination) {
    try {
        fs::remove_all(destination);
        std::cout << "Removed: " << destination.string() << std::endl;
    } catch(const fs::filesystem_error &e) {
        std::cerr << "Deleting folder: " << e.what() << std::endl;
    }
}

void removeExt(const fs::path& src, const std::unordered_map<std::string, fs::path> &destMap) {
    try {
        for(const auto &entry : destMap) {
            const fs::path &destDir = entry.second;
            if(!exists(destDir) || !is_directory(destDir)) {
                std::cerr << "Directory does not exist or is not a directory: " << destDir.string() << std::endl;
                continue;
            }
            for(const auto &entry : fs::directory_iterator(destDir)) {
                if(!entry.is_regular_file()) continue;
                moveFile(entry.path(), src);
            }
            removeFolder(destDir);
        }
    } catch(const fs::filesystem_error &e) {
        std::cerr << e.what() << std::endl;
    }
}

void removeAlph(const fs::path &src) {
    try {
        for(const auto &entry : fs::directory_iterator(src)) {
            const fs::path &destDir = entry.path();
            if(!is_directory(destDir)) {
                std::cerr << "Directory does not exists or is not a directory: " << destDir.string() << std::endl;
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
        std::cerr << e.what() << std::endl;
    }
}

PYBIND11_MODULE(file_sorter, m) {
    m.doc() = "A module for sorting files by extension or alphabetically";
    m.def("move_file", &moveFile,
          "Move a file to a destination directory",
          py::arg("entry"), py::arg("destination"));
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
}