#ifndef UVIDEOOUTPUT_H
#define UVIDEOOUTPUT_H

#include <QObject>
#include <QImage>
#include <QQuickPaintedItem>
#include <QTime>

#include "main.h"
#include "UPlayer.h"

class QPainter;
class UVideoOutput : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QImage mImage READ getImage WRITE setImage)
    Q_PROPERTY(QString fileUrl READ getFileUrl WRITE setFileUrl)
    Q_PROPERTY(int mWidth READ getWidth WRITE setWidth)
    Q_PROPERTY(int mHeight READ getHeight WRITE setHeight)

public:
    UVideoOutput(QQuickItem *parent = 0);
    virtual ~UVideoOutput();

    virtual void paint(QPainter *painter);

    QImage getImage() const;
    void setImage(QImage img);

    QString getFileUrl() const;
    void setFileUrl(QString url);

    int getWidth() const;
    void setWidth(int w);

    int getHeight() const;
    void setHeight(int h);

    Q_INVOKABLE void zoomDraw(float z = 1.0f);

    static void *playVideoTh(void* args);
    static int playVideo2(const char *filepath,UVideoOutput *uVideoOutput);

//    MyThread *myThread;
    UPlayer *uPlayer;

signals:
    void requestUpdate();



public slots:
    void updateBackgroupImage(QString fileUrl);
    void urlPass(QString url);
    void StopPlay();
    void StartPlay();
    void procUpdate();

private:
//   static QImage mImage;
   QImage mImage;
   QString fileUrl;
   int mWidth;
   int mHeight;
   float mZoom;

   //状态,0:设置背景,1:播放视频
   int status=0;



};

#endif // UVIDEOOUTPUT_H
