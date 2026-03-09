#include <iostream>
#include <fstream>

#include "hutils.h"

using std::cout;
using std::string;
namespace u = hUtils;
namespace fs = std::filesystem;

int main() {
    SortAlgo sort;

    u::SetConsoleWindowSize();
    u::text.clearAll();

    const string fileName = "fileSYSman_Config.txt";

    u::log.Action("Checking if config file exists...");
    u::Sleep(1000);
    if(!fs::exists(fileName)) {
        u::log.Error("Config file does not exists!");
        cout << "\nYou don't have the config file for fileSYSman!\n"
             << "Let's quickly configure your settings.\n";
        u::text.toLine();
        u::Pause();
        sort.create(fileName);
    }

    sort.parse(fileName);
    if(sort.getSrcDir().empty()) {
        u::log.Error("Source directory is missing in the config file.");
        u::Pause();
        return 1;
    }

    u::text.toLine();
    fs::path selSrc = pickSrcDir(sort);
    sort.buildDestMap(selSrc);

    mainMenu(sort, fileName, selSrc);
    u::text.toLine();
    u::log.Summary();
    u::text.toLine();
    u::Pause();

    return 0;
}