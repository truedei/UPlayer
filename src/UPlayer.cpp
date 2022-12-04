#include "UPlayer.h"


UPlayer::UPlayer(QObject *parent) : QObject(parent)
{

}

UPlayer::~UPlayer(){

}

QObject *UPlayer::getMSource() const
{

    return m_mSource;
}

void UPlayer::setMSource(QObject *mSource)
{
    m_mSource = mSource;
}

QString UPlayer::getStrBackgroundPath() const
{
    QSettings settingIni(configFileName, QSettings::IniFormat);

    QString backGroundpath = settingIni.value("Path/Background").toString();

    return backGroundpath;
}

void UPlayer::setStrBackgroundPath(const QString &fileUrl)
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
