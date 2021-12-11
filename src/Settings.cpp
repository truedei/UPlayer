#include "Settings.h"


Settings::Settings(QObject *parent) : QObject(parent)
{

}

Settings::~Settings(){

}



int Settings::getVideoFileDialogStatus()
{
    return this->videoFileDialogStatus;
}

void Settings::setVideoFileDialogStatus(const int status)
{
    this->videoFileDialogStatus = status;

    qDebug() <<__FUNCTION__ <<"啦啦啦啦啦" << endl;

    emit updateVideoFileDialogStatus();
}

QString Settings::getStrBackgroundPath() const
{
    QSettings settingIni(configFileName, QSettings::IniFormat);

    QString backGroundpath = settingIni.value("Path/Background").toString();

    return backGroundpath;
}

void Settings::setStrBackgroundPath(const QString &fileUrl)
{

    strBackgroundPath = fileUrl;

    QSettings settingIni(configFileName, QSettings::IniFormat);
    settingIni.setValue("Path/Background",fileUrl);

    QString backGroundpath = settingIni.value("Path/Background").toString();
    if(fileUrl == nullptr)
    {
        if(backGroundpath == nullptr)
        {
            return;
        }
        else
        {
            strBackgroundPath = backGroundpath;
            return;
        }
    }
    if(strBackgroundPath != fileUrl)
    {
        settingIni.setValue("Path/Background",fileUrl);
        strBackgroundPath = fileUrl;
    }
}
