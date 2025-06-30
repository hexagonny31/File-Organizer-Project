#ifndef SORTALGO_H
#define SORTALGO_H

#include "dirManager.h"
#include <unordered_map>

struct sortAlgo {
private:
    std::unordered_map<std::string, std::filesystem::path> destMap;

public:
    //  Moving algorithms.
    void moveFile    (const std::filesystem::path &entry,
                      const std::filesystem::path &destination);
    void moveToSrcDir(const std::filesystem::path &src,
                      const Config &config);
    void byExt   (const std::filesystem::path &src);
    void byAlph  (const std::filesystem::path &src);

    //  Removing algorithms.
    void removeFolder  (const std::filesystem::path &destination);
    void removeExtSort (const std::filesystem::path &src);
    void removeAlphSort(const std::filesystem::path &src);

    std::unordered_map<std::string, std::filesystem::path> getDestMap() const {
        return destMap;
    }
    void setDestMap(const std::unordered_map<std::string, std::filesystem::path>& newDestMap) {
        destMap = newDestMap;
    }
};

#endif