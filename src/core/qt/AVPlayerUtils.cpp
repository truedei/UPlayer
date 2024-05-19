#include "AVPlayerUtils.hpp"

static ConfigUtils config;

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
    return config.getStrBackgroundPath().c_str();
}

void UPlayer::setStrBackgroundPath(const QString &fileUrl)
{
    config.setStrBackgroundPath(fileUrl.toStdString());
}


int UPlayer::setPlayerVolume(int volume){
    return config.setPlayerVolume(volume);
}

int UPlayer::getPlayerVolume(){
    return config.getPlayerVolume();
}

