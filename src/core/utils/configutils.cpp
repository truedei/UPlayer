#include "configutils.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "common.h"

bool ConfigUtils::file_exist(char *name)
{
    struct stat buffer;
    bool ret = (stat(name, &buffer) == 0);
    if (ret == false) {
        DB_PRT("%s is not exist\n", name);
    } else {
        DB_PRT("load and parse %s\n", name);
    }
    return ret;
}

ConfigUtils::ConfigUtils()
{
    char path[100];
    getcwd(path, 100);
    this->configFileUri = path;
    this->configFileUri = this->configFileUri + "/" + this->configFileName;
    DB_PRT("configFileUri:%s\n", this->configFileUri.c_str());


    if (file_exist((char*)(this->configFileUri.c_str()))) {
        std::ifstream in((char*)(this->configFileUri.c_str()));
        in >> config_dom;
        in.close();
    } else {
        return;
    }
}

ConfigUtils::~ConfigUtils()
{
    this->SaveConfig();
}

std::string ConfigUtils::GetConfigFileUri()
{
    return this->configFileUri;
}

int ConfigUtils::setStrBackgroundPath(const string &backGroundPath)
{
    if(backGroundPath == "" || backGroundPath.length() <= 0)
    {
           return -1;
    }
    this->config_dom["backgroundpath"] = backGroundPath.c_str();
    return 1;
}

string ConfigUtils::getStrBackgroundPath()
{
    if(this->config_dom.empty())
    {
        return "";
    }
    return this->config_dom["backgroundpath"];
}

int ConfigUtils::SaveConfig()
{
    std::ofstream ofile;
    ofile.open((char*)(this->configFileUri.c_str()));
    ofile << std::setw(4) << config_dom << std::endl;
    ofile.flush();
    ofile.close();

    return 1;
}

int ConfigUtils::setPlayerVolume(int volume){
    config_dom["volume"] = volume;
    SaveConfig();
    return volume;
}

int ConfigUtils::getPlayerVolume(){
    int volume = 0;
    if(this->config_dom.contains("volume")){
        volume = this->config_dom["volume"];
    }
    return volume;
}
