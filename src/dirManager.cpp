#include "dirManager.h"
#include "hutils.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::string;
namespace fs = std::filesystem;

Config config;

void Config::createConfig(const string &fileName) {
    std::vector<string> dirLabels = {"prefered documents", "downloads", "desktop"};
    std::vector<fs::path> paths(3);
    
    cout << "If you already have your existing documents folder is decently sorted.\n"
         << "I suggest you to create a new secondary documents folder.\n\n";

    for(size_t i = 0; i < dirLabels.size(); ++i) {
        do {
            cout << "Enter your " << dirLabels[i] << " path: ";
            std::cin >> paths[i];

            if(!exists(paths[i]) || !is_directory(paths[i])) {
                cout << "Path does not exists or is not a directory!";
                paths[i].clear();
            }
        } while(paths[i].empty());
    }
    hUtils::log.Success("\nDirectory Accepted:");
    for(const auto &path : paths) {
        cout << '\t' << path << '\n';
    }
    hUtils::sleep(2000);

    std::ofstream file(fileName);

    if(file.is_open()) {
        file << "sourceDirectories=" << paths[0] << '\n';
        file << "initialDirectories=" << paths[1] << "," << paths[2];
        hUtils::log.Success("File created: " + fileName);
    }
    else {
        hUtils::log.Error("Failed to create file " + fileName);
    }
}

void Config::addDirectory(const string &fileName) {
    fs::path newPath;
    std::string input;
    int choice = 0;

    cout << "\nEnter the full path of the directory: ";
    std::cin >> std::ws;
    std::getline(std::cin, input);
    newPath = input;

    if(!fs::exists(newPath) || !fs::is_directory(newPath)) {
        hUtils::log.Error("The specified path does not exist or is not a directory");
        return;
    }

    cout << "\nWhere do you want to add this directory?\n"
         << "1. Source Directories\n"
         << "2. Initial Directories\n"
         << "3. Cancel\n";
         std::cin >> choice;

    switch(choice) {
        case 1:
            srcDir.push_back(newPath);
            hUtils::log.Success("Added " + newPath.string() + " to source directories");
            break;
        case 2:
            initDir.push_back(newPath);
            hUtils::log.Success("Added " + newPath.string() + " to initial directories");
            break;
        case 3:
            cout << "Operation canceled\n";
            break;
        default:
            cout << "Invalid choice. Please enter 1, 2 or 3\n";
    }

    std::ofstream file(fileName);
    if(file.is_open()) {
        file << "sourceDirectories=";
        for (size_t i = 0; i < srcDir.size(); ++i) {
            file << srcDir[i];
            if(i != srcDir.size() -1) file << ",";
        }
        file << '\n';

        file << "initialDirectories=";
        for(size_t i = 0; i < initDir.size(); ++i) {
            file << initDir[i];
            if(i != initDir.size() - 1) file << ",";
        }

        file.close();
        hUtils::log.Success("Cofiguarion file updated successfully");
    } else {
        hUtils::log.Error("Could not update config file");
    }
}

Config Config::readConfig(const string &fileName) {
    std::ifstream file(fileName); Config config;
    if(!file.is_open()) {
        hUtils::log.Error("Failed to open config file: " + fileName);
        return config;
    }
    hUtils::log.Success("Successfully opened: " + fileName);

    string line;
    while(getline(file, line)) {
        hUtils::log.Action("Reading line: ", line);
        if(line.find("sourceDirectories=") == 0) {
            trim(line, "sourceDirectories=", config.srcDir);
        } else if(line.find("initialDirectories=") == 0) {
            trim(line, "initialDirectories=", config.initDir);
        }
    }
    file.close();
    return config;
}

string removeNewLine(const string &str)
{
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");

    if(first == string::npos || last == string::npos){
        return "";
    }

    return str.substr(first, (last - first + 1));
}

void Config::trim(string line, const string dirType, std::vector<fs::path>& dirVector) {
    string dirLine = line.substr(string(dirType).size());
    std::stringstream ss(dirLine);
    string path;

    dirLine = removeNewLine(dirLine);

    while(getline(ss, path, ',')) {
        if(!path.empty() && path.front() == '"' && path.back() == '"') {
            path = path.substr(1, path.size() - 2);
        }
        dirVector.push_back(path);
    }
}