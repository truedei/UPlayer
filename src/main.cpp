#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <QCameraInfo>
#include <QQuickWindow>
#include <QFontDatabase>

#include "UPlayer.h"
#include <QTextCodec>
#include <QQmlContext>

#include "UVideoOutput.h"

int ziti()
{
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
}



int main(int argc, char *argv[])
{


    //设置字体

//    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //

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


    //注册
    qmlRegisterType<UPlayer>("cn.truedei.UPlayer.UPlayer", 1, 0, "UPlayer");
    qmlRegisterType<UVideoOutput>("cn.truedei.UPlayer.UPlayer", 1, 0, "UVideoOutput");

    QQmlApplicationEngine engine;



    engine.load(QUrl(QStringLiteral("qrc:/qml/main2.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;


    return app.exec();
}
