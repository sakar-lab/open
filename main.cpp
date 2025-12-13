#include <iostream>
#include <fstream>
#include <filesystem>
#include <list>
#include <queue>
#include <unistd.h>
#include "configReader.h"

class configWriter {
    std::string fileLoc;
    public:
    configWriter(const std::string &fileLoc){
        this->fileLoc = fileLoc;
    }

    bool writeConfig(configDataT &config_data) const{
        std::ofstream file(fileLoc, std::ios::app);  // open in append mode
        if (!file) {
            std::cerr << "Error opening file!\n";
            return false;
        }
        std::string header = config_data.projName;
        if (!header.empty()) {
            header = '[' +  header + "]\n";
        }
        file << header;

        for (auto &[key, value]: config_data.settings) {
            if (!value.empty() && !key.empty()) {
                file << key << "=" << value << "\n";
            }
        }
        file.close();
        return true;
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

bool isAdding(const std::string& firstArg) {
    if (firstArg.empty()) {
        return false;
    }
    if (firstArg == "-a") {
        return true;
    }
    return false;
}

std::string getCurrentDirectory() {
    return std::filesystem::current_path();
}

int main(const int argc, char* argv[]) {
    auto cfg = configReader(getUserConfigPath(), std::list<std::string>{"ide", "path"});
    if (argc > 1 && std::string(argv[1]) == "--commands") {
        std::cout << "-a\n" << cfg.getAllProjName();
        return 0;
    }

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program name>\n";
        std::cout << "To put more run with "<< argv[0] << " -a\n";
        std::cout << "Proj Options:\n" << cfg.getAllProjName();
        return 1;
    }
    if (isAdding(argv[1])) {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " -a <program name> <ide> <path( '.' for this dir)>\n";
            return 2;
        }
        configDataT data;
        data.projName = argv[2];
        data.settings["ide"] = argv[3];
        data.settings["path"] = argv[4];
        if (data.settings["path"].length() == 1 && data.settings["path"][0] == '.') {
            data.settings["path"] = getCurrentDirectory();
        }
        const auto cfgWriter = configWriter(getUserConfigPath());
        cfgWriter.writeConfig(data);
        return 0;
    }
    configDataT data = cfg.getConfigFor(argv[1]);
    if (data.projName.empty()) {
        std::cerr << "Couldn't find the given Project\n";
        return 1;
    }


    //std::cout << cfg.toString() << std::endl;

    start_in_background(data.settings["ide"],{data.settings["path"]});

    return 0;
}