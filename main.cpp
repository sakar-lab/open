#include <iostream>
#include <fstream>
#include <filesystem>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <unistd.h>
//sudo mv open /usr/local/bin
using namespace std;


typedef struct {
    string projName;
    pmr::map<string, string> settings;
}configDataT;

class configManager {
    string fileLoc;
    list<configDataT> configs;
    list<string> settingTypes;
    public:
    explicit configManager(const string &fileLoc, const optional<list<string>> &settingTypes) {
        this->fileLoc = fileLoc;
        if (settingTypes) {
            this->settingTypes = settingTypes.value();
        }
        if (!ReadConfig()) {
            cout<<"refreshConfig failed"<<endl;
        }
    }

    void addSettingType(const string &settingType) {
        settingTypes.push_back(settingType);
    }

    void addSettingTypes(const list<string> &SettingTypes) {
        for (const string &settingType : SettingTypes) {
            this->addSettingType(settingType);
        }
    }

    optional<configDataT> getConfigFor(const string &configName) {
        for (configDataT &config : configs) {
            if (config.projName == configName) {
                return config;
            }
        }
        return nullopt;
    }

    bool reReadConfig() {
        return ReadConfig();
    }

    string getAllProjName() {
        string output;
        for (configDataT &config : configs) {
            output += config.projName;
            output += "\n";
        }
        return output;
    }

    string toString() {
        string basic_string;
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

    private:
    [[nodiscard]]
   bool ReadConfig(){
        ifstream configFile(fileLoc);
        if (!configFile.is_open()) {
            cerr << "Error opening file " << fileLoc << endl;
            return false;
        }
        string line;
        queue<string> lines;
        while (getline(configFile, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            lines.push(line);
        }
        configFile.close();

        while (true) {
            queue<string> block;
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

    bool isSettingtype(string &text) {
        for (auto &settings: settingTypes) {
            if (text.starts_with(settings)) {
                return true;
            }
        }
        return false;
    }

    bool makeBlock(queue<string> &input, queue<string> &output) {
        while (!input.empty()) {
            if (findHeader(input)) {
                output.push(input.front());
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
    static bool isheader(const string &text) {
        if (text.empty()) return false;
        return text.front() == '[' && text.back() == ']';
    }

    static bool findHeader(queue<string>& text) {
        while (!text.empty()) {
            if (std::string& line = text.front(); !line.empty() && isheader(line)) {
                return true;
            }
            text.pop();
        }
        return false;
    }

    bool blockParse(queue<string> &block, configDataT &output) const {
        if (block.empty()) return false;
        if(findHeader(block)) {
            configDataT tmp;
            string &front = block.front();
            front.pop_back();
            front.erase(front.begin());
            tmp.projName = front;
            block.pop();


            while (!block.empty()) {
                for (auto&key : settingTypes) {
                    front = block.front();
                    if (front.starts_with(key)) {
                        front.erase(0,key.length() + 1); // +1 for =
                        tmp.settings.insert_or_assign(key, front);
                    }
                }
                block.pop();
            }
            output = tmp;
            return true;
        }
        return false;
    }
};


int start_in_background(const std::string& program, const std::vector<std::string>& args) {
    const pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        //child

        std::vector<char*> argv;
        argv.reserve(args.size() + 2);

        argv.push_back(const_cast<char*>(program.c_str())); // argv[0]
        for (const auto& a : args) {
            argv.push_back(const_cast<char*>(a.c_str()));
        }
        argv.push_back(nullptr);

        execvp(program.c_str(), argv.data());

        // If execvp returns, something went wrong
        perror("execvp");
        _exit(1);
    }

    std::cout << "Started process with PID: " << pid << '\n';
    return 0;
}
std::string getUserConfigPath() {
    const char* home = std::getenv("HOME");
    if (!home) {
        throw std::runtime_error("getUserConfigPath failed");
    }
    return std::string(home) + "/.config/open/.config";
}

int main(int argc, char* argv[]) {
    auto cfg = configManager(getUserConfigPath(),list<string>{"ide", "path"});
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << "<program name>\n";
        cout << "To put more run add";
        cout << "Proj Options:\n" << cfg.getAllProjName();
        return 1;
    }
    configDataT data = cfg.getConfigFor(argv[1]).value();


    //cout << cfg.toString() << endl;

    start_in_background(data.settings["ide"],{data.settings["path"]});

    return 0;
}