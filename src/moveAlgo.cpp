#include <iostream>

#include "hutils.h"
#include "sortAlgo.h"

using std::cout;
using std::string;
namespace u = hUtils;
namespace fs = std::filesystem;

void SortAlgo::moveFile(const fs::path &entry, fs::path destination) {
    destination = destination / entry.filename();
    try {
        rename(entry, destination);
        u::log.Moved(destination.string());
        u::log.Action("To: ", entry.string());
    } catch(const fs::filesystem_error &e) {
        u::log.Error("Failed to move " + entry.string() + ": " + string(e.what()));
    }
}

void SortAlgo::toSrcDir(const fs::path &src) {
    for(const auto& init : initDir) {
        if(!exists(init) || !is_directory(init)) continue;
        for(const auto &entry : fs::directory_iterator(init)) {
            if(!entry.is_regular_file()) continue;
            const auto &ext = entry.path().extension().string();
            if(destMap.find(ext) != destMap.end()) moveFile(entry.path(), src);
        }
    }
}

void SortAlgo::byExt(const fs::path &src) {
    try {
        for(const auto& entry : fs::directory_iterator(src)) {
            if(!entry.is_regular_file()) continue;
            const auto &ext = entry.path().extension().string();
            if(destMap.find(ext) != destMap.end()) {
                fs::path destDir = destMap.at(ext);
                if(!exists(destDir)) {
                    try {
                        create_directory(destDir);
                        u::log.Created(destDir.string());
                    } catch(const fs::filesystem_error &e) {
                        u::log.Error("Failed to create folder: " + string(e.what()));
                        continue;
                    }
                }
                moveFile(entry.path(), destDir);
            } else {
                u::log.Warning("Skipped (unmapped extention): " + entry.path().string());
            }
        }
    } catch(const fs::filesystem_error &e) {
        u::log.Error(string(e.what()));
    }
}

void SortAlgo::byAlph(const fs::path &src) {
    try {
        for(const auto &entry : fs::directory_iterator(src)) {
            string name = entry.path().filename().string();
            if(!entry.is_regular_file()) continue;
            if(name.empty() || name[0] == '.') continue;
            if(is_directory(entry)) byAlph(entry.path());
            else {
                char firstChar = toupper(name[0]);
                if(isalpha(firstChar)) {
                    string alphaFolderName(1, firstChar);
                    fs::path alphaFolder = src / alphaFolderName;
                    if(!exists(alphaFolder)) {
                        try {
                            create_directory(alphaFolder);
                            u::log.Created(alphaFolder.string());
                        } catch(const fs::filesystem_error &e) {
                            u::log.Error("Failed to create folder: " + string(e.what()));
                        }
                    }
                    moveFile(entry.path(), alphaFolder);
                }
            }
        }
    } catch(const fs::filesystem_error &e) {
        u::log.Error(string(e.what()));
    }
}