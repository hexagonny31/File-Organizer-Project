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

void printConfig(const Config &config, const sortAlgo &sort, const fs::path &selSrc)
{
    cout << "Current Configuration:\n\n";

    if (!config.getInitDir().empty()) {
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
        if (src.string() == selSrc.string()) {
            cout << hUtils::text.bgColor(104) << src.string() << hUtils::text.defaultText();
        }
        else {
            cout << src.string();
        }
        cout << '\n';
    }

    if (!sort.getDestMap().empty()) {
        cout << "Destination directories (for extensions):\n";
        for (const auto &[extension, destDir] : sort.getDestMap())
            cout << '\t' << destDir.string() << " -> " << extension << '\n';
    }
    else {
        cout << "Destination directories: None specified.\n";
    }
}

fs::path pickSrcDir(const Config &config)
{
    size_t index = 0;
    const auto &srcDir = config.getSrcDir();

    if (srcDir.size() > 1) {
        int choice = -1;
        cout << "\nYou currently have multiple source directories assigned.\n";

        do {
            hUtils::sleep(600);
            cout << "Please pick which folder you want to sort:\n";
            for (size_t i = 0; i < srcDir.size(); ++i) {
                cout << '\t' << i + 1 << " -> " << config.getSrcDir(i).string() << '\n';
            }
            cout << '\n'
                 << hUtils::text.fgColor(93) << "Choose action: " << hUtils::text.defaultText();
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (choice < 1 || static_cast<size_t>(choice) > srcDir.size()) {
                cout << "Invalid choice. Please try again.\n\n";
            }
        } while (choice < 1 || static_cast<size_t>(choice) > srcDir.size());

        index = static_cast<size_t>(choice - 1);
    }
    fs::path selSrc = config.getSrcDir(index);
    hUtils::log.Action("\nSelected source directory: ", selSrc.string());
    return selSrc;
}

int validateChoice(int min, int max, string prompt) {
    int choice = -1;
    string input;

    do{
        std::cout << prompt;
        std::getline(std::cin, input);
        try {
            choice = std::stoi(input);
        } catch(...) {
            choice = 0;
        }
        
        //  Do not change!! This is neccessary and it will break if you try to fix it.
        if(choice < min || choice > max) {
            std::cout << "\nInvalid choice! Please try again!";
            hUtils::sleep(2500);
            hUtils::text.clearAbove(2);
        }
    } while (choice < min || choice > max);
    return choice;
}

void mainMenu(Config &config, sortAlgo &sort, const fs::path &selSrc, const std::string &fileName)
{
    int choice;
    string input;

    std::cout << "1. Move files to your documents folder\n"
              << "2. Sort files in your documents folder\n"
              << "3. Remove sorted folders in your documents folder\n"
              << "4. Add/Remove a directory\n"
              << "5. Exit.\n\n";

    choice = validateChoice(1, 5, hUtils::text.fgColor(93) + "Choose action: " + hUtils::text.defaultText());
    hUtils::text.toLine();

    switch (choice)
    {
        case 1:
            sort.moveToSrcDir(selSrc, config);
            break;
        case 2:
            std::cout << "1. Sort by extension\n"
                      << "2. Sort alphabetiacally\n\n";
            choice = validateChoice(1, 2, hUtils::text.fgColor(93) + "Choose action: " + hUtils::text.defaultText());

            switch (choice) {
                case 1:
                    sort.byExt(selSrc);
                    break;
                case 2:
                    sort.byAlph(selSrc);
                    break;
            }
            break;
        case 3:
            std::cout << "1. Remove extension folders\n"
                      << "2. Remove alphabetical folders\n\n";
            choice = validateChoice(1, 2, hUtils::text.fgColor(93) + "Choose action: " + hUtils::text.defaultText());

            switch (choice) {
                case 1:
                    sort.removeExtSort(selSrc);
                    break;
                case 2:
                    sort.removeAlphSort(selSrc);
                    break;
            }
            break;
        case 4:
            std::cout << "1. Add directory\n"
                      << "2. Remove directory\n\n";
            choice = validateChoice(1, 2, hUtils::text.fgColor(93) + "Choose action: " + hUtils::text.defaultText());

            switch(choice) {
                case 1:
                    config.addDirectory(fileName);
                    break;
                case 2:
                    config.removeDirectory(fileName);
                    break;
            }
            break;
    }
}