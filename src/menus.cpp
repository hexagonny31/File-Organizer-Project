#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unordered_map>

#include "hutils.h"
#include "sortAlgo.h"

using std::cout;
using std::string;
namespace u = hUtils;
namespace fs = std::filesystem;
enum {
    MOVE = 1,
    SORT = 2,
    CONFIGURE = 3,
    EXIT = 4
};

bool proceed() {
    char choice;
    string input;
    do {
        cout << "This action cannot be undone.\n"
             << "Do you want to continue? (y/n): ";
        std::getline(std::cin, input);
        
        if(!input.empty()) choice = std::tolower(input[0]);
        else choice = '\0';

        if(choice != 'y' && choice != 'n') {
            cout << "\n\nInvalid choice! Please try again!";
            u::Sleep(2000);
            u::text.clearAbove(3);
        }
    } while(choice != 'y' && choice != 'n');
    u::text.clearAbove(2);
    return choice == 'y';
}

fs::path pickSrcDir(const SortAlgo &sort) {
    size_t index = 0;
    int lines = 3 + sort.getSrcDir().size();

    if(sort.getSrcDir().size() > 1) {
        size_t choice = -1;
        cout << "You currently have multiple source directories assigned.\n";
        u::Sleep(1000);

        cout << "Please pick which folder you want to sort:\n";
        for(size_t i = 0; i < sort.getSrcDir().size(); ++i) cout << '\t' + std::to_string((i + 1)) + " -> " + sort.getSrcDir(i).string() + '\n';
        choice = u::GetIntegerInput("Choose action: ", 1, sort.getSrcDir().size());
        index = choice - 1;
        u::text.clearAbove(lines);
    }
    fs::path selected = sort.getSrcDir(index);
    hUtils::text.clearAbove(2);
    u::log.Action("Selected source directory: ", selected.string());
    return selected;
}

void printDirectories(const SortAlgo &sort, const fs::path &selSrc) {
    cout << "Current Configuration:\n\n";
    if(!sort.getInitDir().empty()) {
        cout << "Initial directories:\n";
        for(const auto &init : sort.getInitDir()) cout << '\t' + init.string() + '\n';
    }
    else cout << "Initial Directories : None specified.\n";

    cout << "Source directory:\n";
    for(const auto &src : sort.getSrcDir()) {
        cout << '\t';
        std::string srcStr = src.string();
        if(srcStr == selSrc.string()) cout << u::text.bgColor(104) + srcStr + u::text.defaultText();
        else cout << srcStr;
        cout << '\n';
    }

    if(!sort.getDestMap().empty()) {
        int pathLen = 0;
        for(const auto &[extension, destDir] : sort.getDestMap()) {
            int temp = destDir.string().size();
            if(temp > pathLen) pathLen = temp;
        }
        cout << "Destination directories (for extensions):\n";
        for(const auto &[extension, destDir] : sort.getDestMap())
            cout << '\t' << std::setw(pathLen) << std::left << destDir.string() << " -> " + extension + '\n';
    }
    else cout << "Destination directories: None specified.\n";
}

void mainMenu(SortAlgo &sort, const std::string &fileName, fs::path &selSrc) {
    size_t choice;

    do {
        int lines = 7 + sort.getInitDir().size() + sort.getSrcDir().size() + sort.getDestMap().size();
        choice = -1;
        u::text.toLine();
        printDirectories(sort, selSrc);
        u::text.toLine();
        cout << "1. Move files to your documents folder\n"
             << "2. Sort files in your documents folder\n"
             << "3. Configure settings\n"
             << "4. Exit.\n\n";
        choice = u::GetIntegerInput("Choose action: ", 1, 4);
        u::text.clearAbove(6);

        switch(choice) {
        case MOVE:
            if(!proceed()) continue;
            hUtils::text.clearAbove(lines);
            sort.toSrcDir(selSrc);
            break;
        case SORT:
            cout << "1. Sort by extension\n"
                 << "2. Sort alphabetiacally\n"
                 << "3. Remove extension folders\n"
                 << "4. Remove alphabetical folders\n"
                 << "5. Cancel\n\n";
            choice = u::GetIntegerInput("Choose action: ", 1, 5);
            u::text.clearAbove(7);

            switch(choice) {
            case 1:
                u::text.clearAbove(lines);
                sort.byExt(selSrc);
                break;
            case 2:
                u::text.clearAbove(lines);
                sort.byAlph(selSrc);
                break;
            case 3:
                u::text.clearAbove(lines);
                sort.removeExt(selSrc);
                break;
            case 4:
                u::text.clearAbove(lines);
                sort.removeAlph(selSrc);
            case 5:
                cout << "\nOperation Canceled.";
                u::Sleep(1000);
                u::text.clearAbove(2 + lines);
                break;
            }
            break;
        case CONFIGURE:
            cout << "1. Change source directory\n"
                 << "2. Add directory\n"
                 << "3. Add extension\n"
                 << "4. Remove directory\n"
                 << "5. Remove extension\n"
                 << "6. Cancel\n\n";
            choice = u::GetIntegerInput("Choose action: ", 1, 6);
            u::text.clearAbove(8);

            switch(choice) {
            case 1:
                selSrc = pickSrcDir(sort);
                u::text.clearAbove(lines);
                break;
            case 2:
                sort.addDirectory(fileName);
                break;
            case 3:
                sort.addExtension(fileName, selSrc);
                break;
            case 4:
                sort.removeDirectory(fileName);
            case 5:
                sort.removeExtension(fileName, selSrc);
                break;
            case 6:
                cout << "\nOperation Canceled.";
                u::Sleep(1000);
                u::text.clearAbove(2);
                break;
            }
            break;
        case EXIT:
            u::log.Action("Printing sumary...");
            u::Sleep(1000);
            return;
        }
    } while(choice != EXIT);
}