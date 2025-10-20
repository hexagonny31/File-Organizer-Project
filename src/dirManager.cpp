#include <iostream>
#include <fstream>

#include "dirManager.h"
#include "hutils.h"

using std::cout;
using std::string;
namespace u = hUtils;
namespace fs = std::filesystem;

Config config;

void Config::create(const string &fileName) {
    std::vector<string> labels = {"prefered folder", "downloads", "desktop", "documents"};
    std::vector<fs::path> paths(4);

    cout << "\nIf you already have your documents folder is decently sorted,\n"
         << "I suggest you to create a new secondary folder for this program.\n\n";
    for(size_t i = 0; i < labels.size(); ++i) {
        do {
            cout << "Enter your " << labels[i] << " path: ";
            std::cin >> paths[i];

            if(!exists(paths[i]) || !is_directory(paths[i])) {
                cout << "Path does not exists or is not a directory.\n";
                paths[i].clear();
                u::pause(true);
                u::text.clearAbove(3);
            }
        } while(paths[i].empty());
    }
    u::log.Success("\nDirectory Accepted:");
    for(const auto &path : paths) cout << '\t' << path.string() << '\n';
    u::sleep(2000);

    std::ofstream file(fileName);
    if(!file.is_open()) {
        u::log.Error("Failed to create file: " + fileName);
        return;
    }

    std::vector<string> extensions = {".pdf=PDF", ".docx=DOCX", ".doc=DOCX",
                                      ".pptx=PPTX", ".xlsx=XLSX", ".txt=TXT"};
    
    file << "# Config file for document sorter\n\n"
         << "# Source directories where new files will go\n"
         << "srcDir:" + paths[0].string() + "\n"
         << "# Initial directories scanned at startup\n";
    for(size_t i = 1; i < paths.size(); i++) file << "initDir:" + paths[i].string() + '\n';
    file << "\n# Allowed extension to be moved and sorted\n";
    for(const auto &extension : extensions) file << "extension:" + extension + '\n';

    u::text.clearAll();
}

Config& Config::parse(const string &fileName) {
    std::ifstream file(fileName);
    if(!file.is_open()) {
        u::log.Error("Failed to open file: " + fileName);
        return *this;
    }
    u::log.Success("Successfully opened: " + fileName);

    string line;
    while(getline(file, line)) {
        if(line.empty()) continue;
        if(line[0] == '#' || line[0] == ';') continue;

        u::log.Action("Reading Line: ", line);

        if(line.find("srcDir:") == 0) srcDir.push_back(line.substr(7));
        else if(line.find("initDir:") == 0) initDir.push_back(line.substr(8));
        else if(line.find("extension:") == 0) {
            auto position = line.find('=');
            if(position != std::string::npos) {
                std::string key = line.substr(10, position - 10);
                std::string folder = line.substr(position + 1);
                extKeys[key] = folder;
            }
        }
    }
    file.close();
    return *this;
}

void Config::rewrite(const std::string &fileName) {
    std::ofstream file(fileName);
    if(!file.is_open()) {
        u::log.Error("Could not rewrite config file!");
        return;
    }

    file << "# Config file for document sorter\n\n";

    u::log.Action("Rewriting config file: ", "Pasting source directories.");
    file << "# Source directories where new files will go\n";
    for(size_t i = 0; i < srcDir.size(); ++i) file << "srcDir:" + srcDir[i].string() + '\n';

    u::log.Action("Rewriting config file: ", "Pasting initial directories.");
    file << "\n# Initial directories scanned at startup\n";
    for(size_t i = 0; i < initDir.size(); ++i) file << "initDir:" + initDir[i].string() + '\n';

    u::log.Action("Rewriting config file: ", " Pasting extention keys.");
    file << "\n# Allowed extension to be moved and sorted\n";
    for(const auto &[extension, folder] : extKeys) file << "extension:" + extension + "=" + folder + '\n';

    file.close();
    u::log.Success("Configuration file updated successfully!");
}

void Config::addDirectory(const string &fileName) {
    size_t lines = 7 + initDir.size() + srcDir.size() + destMap.size();
    fs::path newPath;
    string input;

    do {
        cout << "Enter the full path of the directory ('E' to exit): ";
        std::cin >> std::ws;
        std::getline(std::cin, input);

        if(input == "E") {
            cout << "\nOperation canceled";
            u::sleep(1000);
            u::text.clearAbove(2 + lines);
            return;
        }

        newPath = input;
        if(!fs::exists(newPath) || !fs::is_directory(newPath)) {
            std::cout << "\nThe specified path does not exits or is not a directory";
            u::sleep(1000);
            u::text.clearAbove(2);
        }
    } while(!fs::exists(newPath) || !fs::is_directory(newPath));
    u::text.clearAbove(1);

    cout << "Where do you want to add this directory?\n"
         << "1. Initial Directories\n"
         << "2. Source Directories\n"
         << "3. Cancel\n\n";
    size_t choice = validateChoice(1, 4, "Choose action: ");

    switch(choice) {
    case 1:
        initDir.push_back(newPath);
        break;
    case 2:
        srcDir.push_back(newPath);
        break;
    case 3:
        cout << "\nOperation canceled";
        u::sleep(1000);
        u::text.clearAbove(8 + lines);
        return;
    }
    u::text.clearAbove(6 + lines);
    u::log.Success("Added " + newPath.string() + " to configuration file");
    rewrite(fileName);
}

void Config::removeDirectory(const string &fileName) {
    int lines = 7 + initDir.size() + srcDir.size() + destMap.size();

    cout << "Where do you want to remove a directory from?\n"
         << "1. Initial Directories\n"
         << "2. Source Directories\n"
         << "3. Cancel\n\n";
    size_t choice = validateChoice(1, 3, "Choose action: ");
    u::text.clearAbove(6);

    std::vector<fs::path> *targetDir = nullptr;

    switch(choice) {
    case 1:
        targetDir = &initDir;
        break;
    case 2:
        targetDir = &srcDir;
        break;
    case 3:
        cout << "Operation canceled";
        u::sleep(1000);
        u::text.clearAbove(1 + lines);
        return;
    }

    if(targetDir->max_size() <= 1) {
        u::text.clearAbove(lines);
        u::log.Warning("You only have 1 assigned " + string(((choice == 1) ? "initial" : "source")) + "directory!");
    }

    cout << "Select the directory to remove:\n";
    for(size_t i = 0; i < targetDir->size(); ++i) cout << std::to_string((i + 1)) + ". " + (*targetDir)[i].string() + '\n';
    int removeIndex = validateChoice(1, targetDir->size(), "\nEnter the number of the directory to remove: ");
    u::text.clearAbove(lines + 3 + targetDir->size());

    fs::path removed = (*targetDir)[removeIndex - 1];
    targetDir->erase(targetDir->begin() + (removeIndex - 1));
    u::log.Success("Removed: " + removed.string());

    rewrite(fileName);
}

void Config::addExtension(const string &fileName, const fs::path &source) {
    int lines = 7 + initDir.size() + srcDir.size() + destMap.size();
    string input;

    cout << "Enter a valid file extention(example: txt or .txt): ";
    std::cin >> std::ws;
    std::getline(std::cin, input);

    if(input == "E") {
        cout << "\nOperation canceled";
        u::sleep(1000);
        u::text.clearAbove(2 + lines);
        return;
    }
    u::text.clearAbove(1 + lines);

    if(input.empty() || input[0] != '.') input.insert(0, ".");
    string folderName = input;
    if(folderName.find('.') == 0)
        folderName.erase(std::remove(folderName.begin(), folderName.end(), '.'), folderName.end());
    folderName = u::text.toUpperCase(folderName);
    u::log.Action("Generated folder name: ", folderName);
    extKeys[input] = folderName;

    u::log.Action("Checking map...", "");
    buildDestMap(source);
    rewrite(fileName);
}

void Config::removeExtension(const string &fileName, const fs::path &source) {
    int lines = 7 + initDir.size() + srcDir.size() + destMap.size();
    std::vector<std::pair<string, string>> items(extKeys.begin(), extKeys.end());

    if(extKeys.max_size() <= 1) {
        u::text.clearAbove(lines);
        u::log.Warning("You only have 1 assigned extension!");
        u::sleep(1000);
        return;
    }

    cout << "Select the extension to remove:\n";
    for(size_t i = 0; i < items.size(); ++i) cout << std::to_string((i + 1)) + ". [" + items[i].first + "] [" + items[i].second + "]\n";
    int removeIndex = static_cast<int>(validateChoice(1, items.size(), "\nEnter the number of the extension to remove: "));
    u::text.clearAbove(3 + items.size() + lines);

    string extToRemove = items[removeIndex - 1].first;
    extKeys.erase(extToRemove);

    u::log.Action("Checking map...", "");
    destMap.clear();
    buildDestMap(source);
    rewrite(fileName);
}

void Config::buildDestMap(const fs::path &source) {
    for(const auto &[extension, folder] : extKeys) {
        u::log.Action("Building Map: ", "Passing coniguration: " + extension);
        fs::path temp = source / folder;
        destMap[extension] = temp;
        if(destMap.find(extension) != destMap.end()) u::log.Action("Building map: ", "Setting destination paths: " + destMap[extension].string());
        else u::log.Error("Failed to set as a destination path: " + temp.string());
    }
}