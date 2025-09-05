#include <iostream>
#include "hutils.h"
#include "sortAlgo.h"

using std::cout;
using std::string;
namespace fs = std::filesystem;

void sortAlgo::removeFolder(const fs::path &destination) {
    try {
        remove_all(destination);
        hUtils::log.Removed(destination.string());
    } catch(const fs::filesystem_error &e) {
        hUtils::log.Error("Deleting folder: " + std::string(e.what()));
    }
}

void sortAlgo::removeExtSort(const fs::path& src, const Config &config) {
    try {
        for(const auto &entry : config.getDestMap()) {
            const fs::path &destDir = entry.second;
            if(!exists(destDir) || !is_directory(destDir)) {
                hUtils::log.Warning("Directory does not exist or is not a directory: " + destDir.string());
                continue;
            }

            for(const auto &entry : fs::directory_iterator(destDir)) {
                const auto& filePath = entry.path();
                if(!entry.is_regular_file()) {
                    hUtils::log.Warning(filePath.string() + " is not a regular file");
                    continue;
                }
                moveFile(filePath, src);
            }
            removeFolder(destDir);
        }
    } catch(const fs::filesystem_error &e) {
        hUtils::log.Error(std::string(e.what()));
    }
}

void sortAlgo::removeAlphSort(const fs::path &src) {
    try {
        for(const auto &entry : fs::directory_iterator(src)) {
            const fs::path &destDir = entry.path();
            if(!is_directory(entry)) {
                hUtils::log.Warning("Directory does not exist or is not a directory: " + destDir.string());
                continue;
            }

            std::string destName = destDir.filename().string();
            if(destName.size() == 1 && isupper(destName[0])) {
                for(const auto &file : fs::directory_iterator(destDir)) {
                    const auto& filePath = file.path();
                    if(!file.is_regular_file()) {
                        hUtils::log.Warning(filePath.string() + " is not a regular file");
                        continue;
                    }
                    moveFile(filePath, src);
                }
                removeFolder(destDir);
            }
        }
    } catch(const fs::filesystem_error &e) {
        hUtils::log.Error(std::string(e.what()));
    }
}