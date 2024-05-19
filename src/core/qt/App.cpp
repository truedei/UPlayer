#include "App.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QFontDatabase>
#include <QQmlContext>

#include "qt/AVPlayerUtils.hpp"
#include "qt/AVPlayerCore.hpp"
#include "qt/AVRecordConnect.hpp"
#include "qt/AVRecordPainted.hpp"

int SetFont()
{
    //设置字体

//    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


//    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));


    //设置字体
//    QString fontsFile = "";
//    fontsFile = "/root/soft/qt-5.9-arm/lib/fonts/DroidSansFallback.ttf"; //linux
//    fontsFile = "E:/嵌入式学习/QT相关/QT字体/DroidSansFallback.ttf"; //windows
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

//    int nIndex = QFontDatabase::addApplicationFont(fontsFile);
//    QString msyh =QFontDatabase::applicationFontFamilies(nIndex).at(0);
//    QFont font(msyh,0);
//    font.setPointSize(10);
//    QGuiApplication::setFont(font);
}

/**
 * 注册class到qml
 *
 * @brief RegisterClass
 */
void RegisterClassToQml(){
    //注册
    qmlRegisterType<UPlayer>("cn.truedei.UPlayer.UPlayer", 1, 0, "UPlayer");
    qmlRegisterType<UVideoOutput>("cn.truedei.UPlayer.UPlayer", 1, 0, "UVideoOutput");
    qmlRegisterType<AVRecordConnect>("cn.truedei.UPlayer.UPlayer", 1, 0, "AVRecordConnect");
    qmlRegisterType<AVRecordPainted>("cn.truedei.UPlayer.UPlayer", 1, 0, "AVRecordPainted");
}

int run_main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // set font
    SetFont();

    //注册
    RegisterClassToQml();

    QQmlApplicationEngine engine;

    //Set the main entrance of qml
    //engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    //engine.load(QUrl(QStringLiteral("qrc:/qml/mediaplay/MediaPlayMainPage.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/qml/MainPage.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
