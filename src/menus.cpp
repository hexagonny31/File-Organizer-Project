#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include "menus.h"
#include "hutils.h"

using std::cout;
using std::string;
namespace fs = std::filesystem;
enum
{
    MOVE = 1,
    SORT = 2,
    CONFIGURE = 3,
    EXIT = 4
};

void printConfig(Config &config, const sortAlgo &sort, const fs::path &selSrc)
{
    cout << "Current Configuration:\n\n";

    if(!config.getInitDir().empty()) {
        cout << "Initial directories:\n";
        for (const auto &init : config.getInitDir())
            cout << '\t' << init.string() << '\n';
    }
    else {
        cout << "Initial Directories: None specified.\n";
    }

    cout << "Source directory:\n";
    for (const auto &src : config.getSrcDir()) {
        cout << '\t';
        if(src.string() == selSrc.string()) {
            cout << hUtils::text.bgColor(104) << src.string() << hUtils::text.defaultText();
        }
        else {
            cout << src.string();
        }
        cout << '\n';
    }

    if(!config.getDestMap().empty()) {
        cout << "Destination directories (for extensions):\n";
        for (const auto &[extension, destDir] : config.getDestMap())
            cout << '\t' << destDir.string() << " -> " << extension << '\n';
    }
    else {
        cout << "Destination directories: None specified.\n";
    }
}

int validateChoice(int min, int max, string prompt)
{
    int choice = -1;
    string input;

    do {
        std::cout << hUtils::text.fgColor(93) << prompt <<  hUtils::text.defaultText();
        std::getline(std::cin, input);
        try
        {
            choice = std::stoi(input);
        }
        catch (...)
        {
            choice = 0;
        }

        //  Do not change!! This is neccessary and it will break if you try to fix it.
        if (choice < min || choice > max)
        {
            std::cout << "\nInvalid choice! Please try again!";
            hUtils::sleep(2500);
            hUtils::text.clearAbove(2);
        }
    } while(choice < min || choice > max);
    return choice;
}

fs::path pickSrcDir(const Config &config)
{
    size_t index = 0;
    const auto &srcDir = config.getSrcDir();
    int lines = 4 + srcDir.size();

    if(srcDir.size() > 1) {
        int choice = -1;
        cout << "You currently have multiple source directories assigned.\n";

        hUtils::sleep(1000);
        cout << "Please pick which folder you want to sort:\n";
        for (size_t i = 0; i < srcDir.size(); ++i) {
            cout << '\t' << i + 1 << " -> " << config.getSrcDir(i).string() << '\n';
        }

        choice = validateChoice(1, srcDir.size(), "Choose action: ");
        index = static_cast<size_t>(choice - 1);
        hUtils::text.clearAbove(lines);
    }
    fs::path selSrc = config.getSrcDir(index);
    hUtils::log.Action("Selected source directory: ", selSrc.string());
    return selSrc;
}

void mainMenu(Config &config, sortAlgo &sort, const fs::path &selSrc, const std::string &fileName)
{
    int lines = 7 + config.getInitDir().size() + config.getSrcDir().size() + config.getDestMap().size();
    size_t choice;

    do {
        size_t choice = 0;
        hUtils::text.toLine();
        printConfig(config, sort, selSrc);
        hUtils::text.toLine();
        std::cout << "1. Move files to your documents folder\n"
                  << "2. Sort files in your documents folder\n"
                  << "3. Configure settings\n"
                  << "4. Exit.\n\n";

        choice = static_cast<size_t>(validateChoice(1, 4,"Choose action: "));
        hUtils::text.clearAbove(6);

        switch(choice) {
        case MOVE:
            hUtils::text.clearAbove(lines);
            sort.moveToSrcDir(selSrc, config);
            break;
        case SORT:
            std::cout << "1. Sort by extension\n"
                    << "2. Sort alphabetiacally\n"
                    << "3. Remove extension folders\n"
                    << "4. Remove alphabetical folders\n\n";
            choice = static_cast<size_t>(validateChoice(1, 4,"Choose action: "));
            hUtils::text.clearAbove(6 + lines);

            switch(choice) {
            case 1:
                sort.byExt(selSrc, config);
                break;
            case 2:
                sort.byAlph(selSrc);
                break;
            case 3:
                sort.removeExtSort(selSrc, config);
                break;
            case 4:
                sort.removeAlphSort(selSrc);
                break;
            }
            break;
        case CONFIGURE:
            std::cout << "1. Add directory\n"
                    << "2. Add extension\n"
                    << "3. Remove directory\n"
                    << "4. Remove extension\n\n";
            choice = static_cast<size_t>(validateChoice(1, 4,"Choose action: "));
            hUtils::text.clearAbove(6);

            switch(choice) {
            case 1:
                config.addDirectory(fileName);
                break;
            case 2:
                config.addExt(selSrc, fileName);
                break;
            case 3:
                config.removeDirectory(fileName);
                break;
            case 4:
                config.removeExt(selSrc, fileName);
                break;
            }
            break;
        case EXIT:
            hUtils::log.Action("Printing summary...","");
            hUtils::sleep(1000);
            return;
        }
    } while(choice != EXIT);
}