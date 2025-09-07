#include <iostream>
#include "hutils.h"
#include "sortAlgo.h"

using std::cout;
using std::string;
namespace fs = std::filesystem;

void sortAlgo::moveFile(const fs::path &entry, const fs::path &destination) {
    fs::path destPath = destination / entry.filename();
    try {
        rename(entry, destPath);
        hUtils::log.Moved(destPath.string());
        hUtils::log.Action("To: ", entry.string());
    } catch(const fs::filesystem_error &e) {
        hUtils::log.Error("Failed to move " + entry.string() + ": " + string(e.what()));
    }
}

void sortAlgo::moveToSrcDir(const fs::path &src, const Config &config) {
    for (const auto& init : config.getInitDir()) {
        if(!exists(init) || !is_directory(init)) continue;

        for(const auto &entry : fs::directory_iterator(init)) {
            const auto &filePath = entry.path();
            if(!entry.is_regular_file()) continue;

            string ext = filePath.extension().string();
            if(config.getDestMap().find(ext) != config.getDestMap().end()) moveFile(filePath, src);
        }
    }
}

void sortAlgo::byExt(const fs::path &src, const Config &config) {
    try {
        for(const auto& entry : fs::directory_iterator(src)) {
            const auto& filePath = entry.path();
            if(!entry.is_regular_file()) continue;

            const auto &ext = filePath.extension().string();

            if(config.getDestMap().find(ext) != config.getDestMap().end()) {
                fs::path destDir = config.getDestMap().at(ext);

                if(!exists(destDir)) {
                    try {
                        create_directory(destDir);
                        hUtils::log.Created(destDir.string());
                    } catch(const fs::filesystem_error &e) {
                        hUtils::log.Error("Failed to create folder: " + string(e.what()));
                        continue;
                    }
                }
                moveFile(filePath, destDir);
            } else {
                hUtils::log.Warning("Skipped (unmapped extension): " + filePath.string());
            }
        }
    } catch(const fs::filesystem_error &e) {
        hUtils::log.Error(string(e.what()));
    }
}

void sortAlgo::byAlph(const fs::path &src) {
    try {
        for(const auto &entry : fs::directory_iterator(src)) {
            const auto& filePath = entry.path();
            string name = filePath.filename().string();
            if(!entry.is_regular_file()) continue;

            if(name.empty() || name[0] == '.') continue;

            if(is_directory(entry)) {
               byAlph(filePath);
            } else {
                char firstChar = toupper(name[0]);
                if(isalpha(firstChar)) {
                    string alphaFolderName(1, firstChar);
                    fs::path alphaFolder = src / alphaFolderName;
                    
                    if(!exists(alphaFolder)) {
                        try {
                            create_directory(alphaFolder);
                            hUtils::log.Created(alphaFolder.string());
                        } catch(const fs::filesystem_error &e) {
                            hUtils::log.Error("Failed to create folder: " + string(e.what()));
                            continue;
                        }
                    }
                    moveFile(filePath, alphaFolder);
                } else {
                    hUtils::log.Warning("Skipped (non-alphabetical start): " + filePath.string());
                }
            }
        }
    } catch(const fs::filesystem_error &e) {
        hUtils::log.Error(string(e.what()));
    }
}