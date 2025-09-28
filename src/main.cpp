#include <iostream>
#include <fstream>

#include "menus.h"
#include "hutils.h"

using std::cout;
using std::string;
namespace u = hUtils;
namespace fs = std::filesystem;

int main() {
    SortAlgo sort;

    u::setConsoleWindowSize();
    u::text.clearAll();

    const string fileName = "fileSYSman_Config.txt";
    bool bufferCheck = false;

    u::log.Action("Checking if config file exists...", "");
    u::sleep(1000);
    if(!fs::exists(fileName)) {
        u::log.Error("Config file does not exists!");
        cout << "\nYou don't have the config file for fileSYSman!\n"
             << "Let's quickly configure your settings.\n";
        u::text.toLine();
        u::pause();
        sort.create(fileName);
        bufferCheck = true;
    }

    sort.parse(fileName);
    if(sort.getSrcDir().empty()) {
        u::log.Error("Source directory is missing in the config file.");
        u::pause(bufferCheck);
        return 1;
    }

    u::text.toLine();
    fs::path selSrc = pickSrcDir(sort);
    sort.buildDestMap(selSrc);

    mainMenu(sort, fileName, selSrc);
    u::text.toLine();
    u::log.Summary();
    u::text.toLine();
    u::pause(true);

    return 0;
}