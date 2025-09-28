#ifndef DIRMANAGER_H
#define DIRMANAGER_H

#include <vector>
#include <filesystem>
#include <unordered_map>

struct Config {
protected:
    std::vector<std::filesystem::path> srcDir;
    std::vector<std::filesystem::path> initDir;
    std::unordered_map<std::string, std::string> extKeys;
    std::unordered_map<std::string, std::filesystem::path> destMap;

    void rewrite(const std::string &fileName);
public:
    void    create         (const std::string &fileName);
    Config& parse          (const std::string &fileName);
    void    addDirectory   (const std::string &fileName);
    void    removeDirectory(const std::string &fileName);
    void    addExtension   (const std::string &fileName, const std::filesystem::path &source);
    void    removeExtension(const std::string &fileName, const std::filesystem::path &source);
    void    buildDestMap   (const std::filesystem::path &source);

    const std::vector<std::filesystem::path>& getSrcDir() const { return srcDir; }
    std::filesystem::path getSrcDir(size_t index) const {
        if(index < getSrcDir().size()) return getSrcDir()[index];
        else throw std::out_of_range("Index out of range in getSrcDirAt");
    }
    const std::vector<std::filesystem::path>& getInitDir() const { return initDir; }
    const std::unordered_map<std::string, std::filesystem::path>& getDestMap() const { return destMap; }
};

#endif