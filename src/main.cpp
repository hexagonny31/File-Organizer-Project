#include <iostream>
#include <fstream>
#include <unordered_map>
#include "menus.h"
#include "hutils.h"

using std::cout;
using std::string;
namespace fs = std::filesystem;

int main() {
    Config config;
    sortAlgo sort;

    hUtils::setConsoleWindowSize(); hUtils::text.clearAll();
    hUtils::text.toLine();

    const std::string fileName = "fileSYSman_Config.txt";
    bool check = false;

    hUtils::log.Action("Checking if config file exists...", "");
    hUtils::sleep(1000);
    if(!fs::exists(fileName)) {
        hUtils::log.Error("Config file does not exist!");
        std::cout << "\nYou don't have the config file for fileSYSman!\n"
                  << "Let's quickly configure your settings.\n\n";
        hUtils::pause();
        hUtils::text.toLine();
        std::cout << '\n';
        config.createConfig(fileName);
        check = true;
    }

    config = config.readConfig(fileName);
    const auto& srcDir = config.getSrcDir();
    
    if(srcDir.empty()) {
        hUtils::log.Error("Source directory is missing in the config file.");
        hUtils::pause(check);
        return 1;
    }

    fs::path selSrc = pickSrcDir(config);
    std::unordered_map<string, fs::path> tempMap = {
        {".pdf",  selSrc / "PDF"},
        {".docx", selSrc / "DOCX"},
        {".doc",  selSrc / "DOCX"},
        {".pptx", selSrc / "PPTX"},
        {".xlsx", selSrc / "XLSX"}
    };
    sort.setDestMap(tempMap);

    hUtils::sleep(2000); hUtils::text.clearAll();
    hUtils::text.toLine();
    printConfig(config, sort, selSrc);
    hUtils::pause();

    mainMenu(config, sort, selSrc, fileName);

    hUtils::log.Summary();
    hUtils::pause(true);

    return 0;
}