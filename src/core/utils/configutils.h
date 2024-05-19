#ifndef CONFIGUTILS_H
#define CONFIGUTILS_H

#include "string"
#include <stdio.h>
#include <unistd.h>

#include "nlohmann/json.hpp"
using nlohmann::json;
using namespace std;

class ConfigUtils
{

private:
    //背景图片path
    std::string strBackgroundPath;

    // config file path
    std::string configFileUri;

    // config file name
    std::string configFileName = "UplayerSetting.json";

    json config_dom;

    bool file_exist(char *name);

public:
    ConfigUtils();
    ~ConfigUtils();

    std::string GetConfigFileUri();

    int setStrBackgroundPath(const std::string &backGroundPath);
    std::string getStrBackgroundPath();

    int SaveConfig();


    int setPlayerVolume(int volume);
    int getPlayerVolume();


};

#endif // CONFIGUTILS_H
