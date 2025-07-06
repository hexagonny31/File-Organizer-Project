#ifndef DIRMANAGER_H
#define DIRMANAGER_H

#include <vector>
#include <filesystem>
#include <unordered_map>

struct Config {
private:
    std::vector<std::filesystem::path> srcDir;
    std::vector<std::filesystem::path> initDir;

    void trim(std::string line,
              const std::string dirType,
              std::vector<std::filesystem::path> &dirVector);

public:
    void createConfig   (const std::string &fileName);
    void addDirectory   (const std::string &fileName);
    void removeDirectory(const std::string &fileName);
    Config readConfig   (const std::string &fileName);

    const std::vector<std::filesystem::path>& getSrcDir() const { return srcDir; }
    std::filesystem::path getSrcDir(size_t index) const { 
        if(index < getSrcDir().size()) {
            return getSrcDir()[index];
        } else {
            throw std::out_of_range("Index out of range in getSrcDirAt");
        }
    }
    void setSrcDir(const std::vector<std::filesystem::path>& newSrcDir) { srcDir = newSrcDir; }
    void addSrcDir(const std::filesystem::path& path) { srcDir.push_back(path); }

    const std::vector<std::filesystem::path>& getInitDir() const { return initDir; }
    void setInitDir(const std::vector<std::filesystem::path>& newInitDir) { initDir = newInitDir; }
    void addInitDir(const std::filesystem::path& path) { initDir.push_back(path); }
};

#endif