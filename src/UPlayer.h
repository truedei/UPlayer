#ifndef UPLAYER_H
#define UPLAYER_H

#include <QMainWindow>
#include <QObject>
#include <QQuickItem>

#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include "main.h"

class UPlayer : public QObject
{
    Q_OBJECT
public:
    explicit UPlayer(QObject *parent = nullptr);
    ~UPlayer();

private:
    //背景图片path
    QString strBackgroundPath;
    //QCoreApplication::applicationDirPath() 获取可执行文件所在路径
    QString configFileName = QCoreApplication::applicationDirPath() + "/setting.ini";

    QObject*                                    m_mSource{nullptr};

public:

    void setStrBackgroundPath(const QString &fileUrl);

    QObject *source() const;
    QObject *getMSource() const;
    void setMSource(QObject *mSource);

public slots:
    QString getStrBackgroundPath() const;


};

#endif // UPLAYER_H
