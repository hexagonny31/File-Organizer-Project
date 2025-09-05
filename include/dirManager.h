#ifndef DIRMANAGER_H
#define DIRMANAGER_H

#include <vector>
#include <filesystem>
#include <unordered_map>

struct Config {
private:
    std::vector<std::filesystem::path> srcDir;
    std::vector<std::filesystem::path> initDir;
    std::unordered_map<std::string, std::string> extToFolder;
    std::unordered_map<std::string, std::filesystem::path> destMap;
    
    void rewriteConfigFile(const std::string &fileName);
    void trim(std::string line,
              const std::string dirType,
              std::vector<std::filesystem::path> &dirVector);

public:
    void createConfigFile (const std::string &fileName);
    Config parseConfigFile(const std::string &fileName);
    void addDirectory     (const std::string &fileName);
    void removeDirectory  (const std::string &fileName);

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
    void buildDestMap(const std::filesystem::path &src, Config &config);
    const std::unordered_map<std::string, std::filesystem::path>& getDestMap() const { return destMap; }
    
    void addExt(const std::filesystem::path &src, const std::string &fileName, Config &config);
    void removeExt(const std::filesystem::path &src, const std::string &fileName, Config &config);
    const auto& getExtToFolder() const { return extToFolder; }
};

#endif