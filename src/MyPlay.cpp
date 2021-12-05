#include "MyPlay.h"
#include "QSettings"
#include "qdebug.h"

MyPlay::MyPlay(QObject *parent) : QObject(parent)
{

    setBackGround(NULL);
}


void MyPlay::changeBackground(QString url){

    qDebug() << "select url1=" << url <<endl;
    setBackGround(url);
    return ;
}


QString MyPlay::backGround() const{

    return m_strBackGroupundPath;
}


QString MyPlay::backGroundChoose() const{

    return m_strBackGroupundChoosePath;
}


void MyPlay::setBackGround(QString url){

    qDebug() << "select url2=" << url <<endl;

    QSettings * settingini = new QSettings("setting.ini",QSettings::IniFormat);

    QString backGroundpath = settingini->value("Path/Background").toString();
    if(url == NULL)
    {
        if(backGroundpath == NULL){
            delete settingini;
            return ;
        }else {
            m_strBackGroupundPath = backGroundpath;
            m_strBackGroupundChoosePath = backGroundpath;
            delete  settingini;

            //通知UI层
            emit backGroundChooseChanged(m_strBackGroupundPath);
            emit backGroundChooseChanged(m_strBackGroupundChoosePath);
        }
    }
    else{
        if(url != m_strBackGroupundPath)
        {
            settingini->setValue("Path/Background",url);
            m_strBackGroupundPath = url;
            m_strBackGroupundChoosePath = url;
            //通知UI层
            emit backGroundChooseChanged(m_strBackGroupundPath);
            emit backGroundChooseChanged(m_strBackGroupundChoosePath);
        }
    }

    return ;
}


