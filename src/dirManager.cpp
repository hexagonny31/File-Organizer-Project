#include "dirManager.h"
#include "menus.h"
#include "hutils.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::string;
namespace fs = std::filesystem;

Config config;

void Config::createConfigFile(const string &fileName) {
    std::vector<string> dirLabels = {"prefered documents", "downloads", "desktop"};
    std::vector<fs::path> paths(3);

    cout << "\nIf you already have your existing documents folder is decently sorted,\n"
         << "I suggest you to create a new secondary documents folder.\n\n";

    for(size_t i = 0; i < dirLabels.size(); ++i) {
        do {
            cout << "Enter your " << dirLabels[i] << " path: ";
            std::cin >> paths[i];

            if (!exists(paths[i]) || !is_directory(paths[i])) {
                cout << "Path does not exists or is not a directory!\n";
                paths[i].clear();
            }
        } while (paths[i].empty());
    }

    hUtils::log.Success("\nDirectory Accepted:");
    for(const auto &path : paths) {
        cout << '\t' << path.string() << '\n';
    }
    hUtils::sleep(2000);

    std::ofstream file(fileName);

    if(file.is_open()) {
        std::vector<string> defaultExtensions = {".pdf=PDF", ".docx=DOCX", ".doc=DOCX",
                                                 ".pptx=PPTX", ".xlsx=XLSX", ".txt=TXT"};

        file << "# Config file for document sorter\n";
        file << "# Source directory where new files will go\n";
        file << "sourceDirectories=" << paths[0].string();
        file << "\n# Initial directories scanned at startup\n";
        file << "initialDirectories=" << paths[1].string() << "," << paths[2].string();
        file << "\n\n# Default extension mappings\n";
        for (const auto &ext : defaultExtensions)
        {
            file << "extension:" << ext << '\n';
        }
        hUtils::log.Success("File created: " + fileName);
    }
    else {
        hUtils::log.Error("Failed to create file " + fileName);
    }
    hUtils::text.clearAll();
}

Config Config::parseConfigFile(const string &fileName) {
    std::ifstream file(fileName);
    Config config;
    if(!file.is_open()) {
        hUtils::log.Error("Failed to open config file: " + fileName);
        return config;
    }
    hUtils::log.Success("Successfully opened: " + fileName);

    string line;
    while (getline(file, line))
    {
        if(line.empty()) continue;
        if (line[0] == '#' || line[0] == ';') continue;

        hUtils::log.Action("Reading line: ", line);

        if(line.find("sourceDirectories=") == 0) {
            trim(line, "sourceDirectories=", config.srcDir);
        }
        else if(line.find("initialDirectories=") == 0) {
            trim(line, "initialDirectories=", config.initDir);
        }
        else if(line.find("extension:") == 0) {
            auto pos = line.find('=');
            if(pos != std::string::npos) {
                std::string key = line.substr(10, pos - 10);
                std::string folder = line.substr(pos + 1);
                config.extToFolder[key] = folder;
            }
        }
    }
    file.close();
    return config;
}

void Config::rewriteConfigFile(const std::string &fileName) {
    std::ofstream file(fileName);
    if(!file.is_open()) {
        hUtils::log.Error("Could not rewrite config file!");
        return;
    }

    hUtils::log.Action("Rewriting config file: ", "Pasting source directories.");
    file << "# Config file for document sorter\n";
    file << "# Source directory where new files will go\n";
    file << "sourceDirectories=";
    for(size_t i = 0; i < srcDir.size(); ++i) {
        file << srcDir[i].string();
        if (i != srcDir.size() - 1)
            file << ",";
    }

    hUtils::log.Action("Rewriting config file: ", "Pasting initial directories.");
    file << "\n# Initial directories scanned at startup\n";
    file << "initialDirectories=";
    for(size_t i = 0; i < initDir.size(); ++i) {
        file << initDir[i].string();
        if (i != initDir.size() - 1)
            file << ",";
    }

    hUtils::log.Action("Rewriting config file: ", "Pasting extensions and folder names.");
    file << "\n\n# Default extension mappings\n";
    for(const auto &[ext, folder] : extToFolder) {
        file << "extension:" << ext << "=" << folder << '\n';
    }

    file.close();
    hUtils::log.Success("Configuration file updated successfully!");
}

void Config::addDirectory(const string &fileName) {
    int lines = 7 + getInitDir().size() + getSrcDir().size() + getDestMap().size();
    fs::path newPath;
    std::string input;

    cout << "Enter the full path of the directory: ";
    std::cin >> std::ws;
    std::getline(std::cin, input);
    newPath = input;
    hUtils::text.clearAbove(1);

    if(!fs::exists(newPath) || !fs::is_directory(newPath)) {
        hUtils::log.Error("The specified path does not exist or is not a directory");
        hUtils::sleep(1000);
        hUtils::text.clearAbove(1);
        return;
    }

    cout << "Where do you want to add this directory?\n"
         << "1. Initial Directories\n"
         << "2. Source Directories\n"
         << "3. Cancel\n\n";
    int choice = validateChoice(1, 3, hUtils::text.fgColor(93) + "Choose action: " + hUtils::text.defaultText());
    hUtils::text.clearAbove(6);

    switch(choice) {
    case 1:
        initDir.push_back(newPath);
        break;
    case 2:
        srcDir.push_back(newPath);
        break;
    case 3:
        cout << "Operation canceled\n";
        hUtils::sleep(1000);
        hUtils::text.clearAbove(1);
        return;
    default:
        cout << "Invalid choice. Please enter 1, 2 or 3\n";
        hUtils::sleep(1000);
        hUtils::text.clearAbove(1);
        return;
    }
    hUtils::text.clearAbove(lines);
    hUtils::log.Success("Added " + newPath.string() + " to config file");
    rewriteConfigFile(fileName);
}

void Config::removeDirectory(const string &fileName) {
    int lines = 7 + getInitDir().size() + getSrcDir().size() + getDestMap().size();

    cout << "Where do you want to remove a directory from?\n"
         << "1. Initial Directories\n"
         << "2. Source Directories\n"
         << "3. Cancel\n\n";
    int choice = validateChoice(1, 3, hUtils::text.fgColor(93) + "Choose action: " + hUtils::text.defaultText());
    hUtils::text.clearAbove(6);

    std::vector<fs::path> *targetDir = nullptr;
    string dirType;

    switch (choice)
    {
    case 1:
        targetDir = &initDir;
        dirType = "initial";
        break;
    case 2:
        targetDir = &srcDir;
        dirType = "source";
        break;
    case 3:
        cout << "Operation canceled\n";
        hUtils::sleep(1000);
        hUtils::text.clearAbove(1);
        return;
    default:
        cout << "Invalid choice. Please enter 1, 2, or 3\n";
        hUtils::sleep(1000);
        hUtils::text.clearAbove(1);
        return;
    }

    if(targetDir->max_size() <= 1) {
        hUtils::log.Warning("You only have 1 assigned " + dirType + " directory!");
        hUtils::sleep(1000);
        hUtils::text.clearAbove(1);
        return;
    }

    cout << "Select the directory to remove:\n";
    for(size_t i = 0; i < targetDir->size(); ++i) {
        cout << '\t' << i + 1 << ". " << (*targetDir)[i].string() << '\n';
    }
    int removeIndex = validateChoice(1, targetDir->size(), "\nEnter the number of the directory to remove: ");
    hUtils::text.clearAbove(lines + 3 + targetDir->size());

    fs::path removed = (*targetDir)[removeIndex - 1];
    targetDir->erase(targetDir->begin() + (removeIndex - 1));
    hUtils::log.Success("Removed: " + removed.string());

    rewriteConfigFile(fileName);
}

void Config::addExt(const fs::path &src, const string &fileName, Config &config) {
    string input;

    cout << "Enter a valid file extention(example: txt or .txt): ";
    std::cin >> std::ws;
    std::getline(std::cin, input);

    if(input.empty() || input[0] != '.') input.insert(0, ".");

    string folderName = input;
    if(folderName.find('.') == 0)
        folderName.erase(std::remove(folderName.begin(), folderName.end(), '.'), folderName.end());
    folderName = hUtils::text.toUpperCase(folderName);
    hUtils::log.Action("\nGenerated folder name: ", folderName);
    config.extToFolder[input] = folderName;

    auto &map = config.getExtToFolder();
    hUtils::log.Action("Checking map...", "");
    cout << "Map size: " << map.size() << '\n';
    cout << "Elements:\n";
    for(const auto &[ext, folder] : map) {
        cout << "\t[" << ext << "] [" << folder << "]\n";
    }

    rewriteConfigFile(fileName);
}

void Config::removeExt(const fs::path &src, const string &fileName, Config &config) {
    auto &map = config.getExtToFolder();
    std::vector<std::pair<std::string, std::string>> items(
        map.begin(), map.end());

    if(config.extToFolder.max_size() <= 1) {
        hUtils::log.Warning("You only have 1 assigned extension!");
        hUtils::sleep(4000);
        return;
    }

    cout << "\nSelect the extension to remove:\n";
    for(size_t i = 0; i < items.size(); ++i) {
        std::cout << i + 1 << ". [" << items[i].first << "] [" << items[i].second << "]\n";
    }

    size_t removeIndex;
    cout << "\nEnter the number of the directory to remove: ";
    std::cin >> removeIndex;
    hUtils::text.toLine();

    if(removeIndex < 1 || removeIndex > items.size()) {
        hUtils::log.Error("Invalid index.");
        return;
    }

    string extToRemove = items[removeIndex - 1].first;
    extToFolder.erase(extToRemove);

    hUtils::log.Action("Checking map...", "");
    cout << "Map size: " << map.size() << '\n';
    cout << "Elements:\n";
    for(const auto &[ext, folder] : map) {
        cout << "\t[" << ext << "] [" << folder << "]\n";
    }

    rewriteConfigFile(fileName);
}

void Config::buildDestMap(const fs::path &src, Config &config) {
    for (const auto &[ext, folder] : config.getExtToFolder())
    {
        hUtils::log.Action("Building Map: ", "Passing configuration: " + ext);
        fs::path temp = src / folder;
        config.destMap[ext] = temp;
        if(config.destMap.find(ext) != config.destMap.end()) {
            hUtils::log.Action("Building Map: ", "Setting destination paths: " + config.destMap[ext].string());
        }
        else {
            hUtils::log.Error("Failed to set as a destination path: " + temp.string());
        }
    }
}

string removeNewLine(const string &str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");

    if (first == string::npos || last == string::npos)
    {
        return "";
    }

    return str.substr(first, (last - first + 1));
}

void Config::trim(string line, const string dirType, std::vector<fs::path> &dirVector) {
    string dirLine = line.substr(string(dirType).size());
    std::stringstream ss(dirLine);
    string path;

    dirLine = removeNewLine(dirLine);

    while(getline(ss, path, ',')) {
        dirVector.push_back(path);
    }
}