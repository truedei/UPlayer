#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include <QObject>
#include <QQuickItem>

#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include "main.h"

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int videoFileDialogStatus READ getVideoFileDialogStatus NOTIFY updateVideoFileDialogStatus);

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

private:
    //背景图片path
    QString strBackgroundPath;
    //中间打开提示框,0:open,1:close
    int videoFileDialogStatus = 0;

    //QCoreApplication::applicationDirPath() 获取可执行文件所在路径
    QString configFileName = QCoreApplication::applicationDirPath() + "/setting.ini";





public:

    void setStrBackgroundPath(const QString &fileUrl);


public slots:
    QString getStrBackgroundPath() const;

    int getVideoFileDialogStatus();
    void setVideoFileDialogStatus(const int status);



signals:
    void updateVideoFileDialogStatus();



};

#endif // SETTINGS_H
