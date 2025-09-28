#include <iostream>

#include "hutils.h"
#include "sortAlgo.h"

using std::cout;
using std::string;
namespace u = hUtils;
namespace fs = std::filesystem;

void SortAlgo::removeFolder(const fs::path &destination) {
    try {
        remove_all(destination);
        u::log.Removed(destination.string());
    } catch(const fs::filesystem_error &e) {
        u::log.Error("Deleting folder: " + std::string(e.what()));
    }
}

void SortAlgo::removeExt(const fs::path& src) {
    try {
        for(const auto &entry : destMap) {
            const fs::path &destDir = entry.second;
            if(!exists(destDir) || !is_directory(destDir)) {
                u::log.Warning("Directory does not exist or is not a directory: " + destDir.string());
                continue;
            }
            for(const auto &entry : fs::directory_iterator(destDir)) {
                if(!entry.is_regular_file()) continue;
                moveFile(entry.path(), src);
            }
            removeFolder(destDir);
        }
    } catch(const fs::filesystem_error &e) {
        u::log.Error(std::string(e.what()));
    }
}

void SortAlgo::removeAlph(const fs::path &src) {
    try {
        for(const auto &entry : fs::directory_iterator(src)) {
            const fs::path &destDir = entry.path();
            if(!is_directory(destDir)) {
                u::log.Warning("Directory does not exists or is not a directory: " + destDir.string());
                continue;
            }
            std::string destName = destDir.filename().string();
            if(destName.size() == 1 && isupper(destName[0])) {
                for(const auto &entry : fs::directory_iterator(destDir)) {
                    if(!entry.is_regular_file()) continue;
                    moveFile(entry.path(), src);
                }
                removeFolder(destDir);
            }
        }
    } catch(const fs::filesystem_error &e) {
        u::log.Error(std::string(e.what()));
    }
}