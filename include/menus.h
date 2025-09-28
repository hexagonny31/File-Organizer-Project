#ifndef MENUS_H
#define MENUS_H

#include <filesystem>
#include "sortAlgo.h"

size_t validateChoice(size_t min, size_t max, std::string prompt);
void printDirectories(const SortAlgo &sort,
                      const std::filesystem::path &selSrc);
std::filesystem::path pickSrcDir(const SortAlgo &sort);
void mainMenu   (SortAlgo &sort,
                 const std::string &fileName,
                 std::filesystem::path &selSrc);

#endif