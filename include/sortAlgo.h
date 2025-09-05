#ifndef SORTALGO_H
#define SORTALGO_H

#include "dirManager.h"
#include <unordered_map>

struct sortAlgo {
public:
    //  Moving algorithms.
    void moveFile    (const std::filesystem::path &entry,
                      const std::filesystem::path &destination);
    void moveToSrcDir(const std::filesystem::path &src,
                      const Config &config);
    void byExt   (const std::filesystem::path &src,
                  const Config &config);
    void byAlph  (const std::filesystem::path &src);

    //  Removing algorithms.
    void removeFolder  (const std::filesystem::path &destination);
    void removeExtSort (const std::filesystem::path &src,
                        const Config &config);
    void removeAlphSort(const std::filesystem::path &src);
};

#endif