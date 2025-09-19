#ifndef MENUS_H
#define MENUS_H

#include <filesystem>
#include "sortAlgo.h"

int validateChoice(int min, int max, std::string prompt);
void printConfig(Config &config,
                 const sortAlgo &sort,
                 const std::filesystem::path &selSrc);
std::filesystem::path pickSrcDir(const Config &config);
void mainMenu   (Config &config,
                 sortAlgo &sort,
                 std::filesystem::path &selSrc,
                 const std::string &fileName);

#endif