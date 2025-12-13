#include "configReader.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>

configReader::configReader(const std::string &fileLoc, const std::optional<std::list<std::string> > &settingTypes) {
    this->fileLoc = fileLoc;
    if (settingTypes) {
        this->settingTypes = settingTypes.value();
    }
    if (!ReadConfig()) {
        std::cout<<"refreshConfig failed"<< std::endl;
    }
}

void configReader::addSettingType(const std::string &settingType) {
    settingTypes.push_back(settingType);
}

void configReader::addSettingTypes(const std::list<std::string> &SettingTypes) {
    for (const std::string &settingType : SettingTypes) {
        this->addSettingType(settingType);
    }
}

configDataT configReader::getConfigFor(const std::string &configName) {
    for (configDataT &config : configs) {
        if (config.projName == configName) {
            return config;
        }
    }
    configDataT config;
    return config;
}

bool configReader::reReadConfig() {
    return ReadConfig();
}

std::string configReader::getAllProjName() {
    std::string output;
    for (configDataT &config : configs) {
        output += config.projName;
        output += "\n";
    }
    return output;
}

std::string configReader::toString() {
    std::string basic_string;
    for (auto &[projName, settings]: configs) {

        basic_string.append(projName);
        basic_string.append("\n");
        for (auto &[key, value] : settings) {
            basic_string.append(key);
            basic_string.append("=");
            basic_string.append(value);
            basic_string.append("\n");
        }
        basic_string.append("=======================================================\n");
    }
    return basic_string;
}

bool configReader::ReadConfig() {
    std::ifstream configFile(fileLoc);
    if (!configFile.is_open()) {
        std::cerr << "Error opening file " << fileLoc << std::endl;
        return false;
    }
    std::string line;
    std::queue<std::string> lines;
    while (getline(configFile, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push(line);
    }
    configFile.close();

    while (true) {
        std::queue<std::string> block;
        if (!makeBlock(lines, block)) {
            break;
        }
        configDataT Data;
        if (!blockParse(block, Data)) {
            break;
        }
        configs.push_back(Data);
    }
    return true;
}

bool configReader::isSettingtype(const std::string &text) const
{
    return std::ranges::any_of(settingTypes, [&](const std::string_view s) {
       return text.starts_with(s);
   });
}

bool configReader::makeBlock(std::queue<std::string> &input, std::queue<std::string> &output) const
{
    while (!input.empty()) {
        if (findHeader(input)) {
            output.push(input.front()); //alloced at
            input.pop();
            while (!input.empty()) {
                if (!isSettingtype(input.front())) {
                    if (isheader(input.front())) {
                        break;
                    }
                    input.pop();
                    continue;
                }
                output.push(input.front());
                input.pop();
            }
            return true;
        }
    }
    return false;
}

bool configReader::isheader(const std::string &text) {
    if (text.empty()) {
        return false;
    }
    return text.front() == '[' && text.back() == ']';
}

bool configReader::findHeader(std::queue<std::string> &text) {
    while (!text.empty()) {
        if (std::string& line = text.front(); !line.empty() && isheader(line)) {
            return true;
        }
        text.pop();
    }
    return false;
}

bool configReader::blockParse(std::queue<std::string> &block,
                              configDataT &output) const
{
    if (block.empty()) return false;
    if (!findHeader(block)) return false;
    configDataT tmp;

    std::string header = block.front();
    block.pop();

    header.pop_back();
    header.erase(header.begin());
    tmp.projName = header;

    while (!block.empty()) {
        std::string line = block.front();
        block.pop();

        for (const auto& key : settingTypes) {
            if (line.starts_with(key)) {
                tmp.settings[key] = line.substr(key.length() + 1);
                break;
            }
        }
    }
    output = std::move(tmp);
    return true;
}

