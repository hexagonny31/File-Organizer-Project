#ifndef SORTALGO_H
#define SORTALGO_H

#include <unordered_map>
#include "dirManager.h"

struct SortAlgo : Config {
public:
    // I hope what I'm doing is correct.
    SortAlgo& parse(const std::string &fileName) {
        Config::parse(fileName);
        return *this;
    }

    //  Moving algorithms.
    void moveFile    (const std::filesystem::path &entry,
                      std::filesystem::path destination);
    void toSrcDir    (const std::filesystem::path &src);
    void byExt       (const std::filesystem::path &src);
    void byAlph      (const std::filesystem::path &src);

    //  Removing algorithms.
    void removeFolder(const std::filesystem::path &destination);
    void removeExt   (const std::filesystem::path &src);
    void removeAlph  (const std::filesystem::path &src);
};

#endif