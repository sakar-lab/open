#ifndef CONFIGREADER_H
#define CONFIGREADER_H
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <string>
typedef struct {
    std::string projName;
    std::pmr::map<std::string, std::string> settings;
}configDataT;

class configReader {
public:
    explicit configReader(const std::string &fileLoc, const std::optional<std::list<std::string>> &settingTypes);
    void addSettingType(const std::string &settingType);
    void addSettingTypes(const std::list<std::string> &SettingTypes);

    configDataT getConfigFor(const std::string &configName);
    bool reReadConfig();
    std::string getAllProjName();
    std::string toString();

private:
    std::string fileLoc;
    std::list<configDataT> configs;
    std::list<std::string> settingTypes;
    bool ReadConfig();
    [[nodiscard]] bool isSettingtype(const std::string &text) const;
    bool makeBlock(std::queue<std::string> &input, std::queue<std::string> &output) const;
    static bool isheader(const std::string &text);
    static bool findHeader(std::queue<std::string>& text);
    bool blockParse(std::queue<std::string> &block, configDataT &output) const;
};


#endif //CONFIGREADER_H