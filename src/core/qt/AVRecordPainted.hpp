#ifndef AVRECORDPAINTED_H
#define AVRECORDPAINTED_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QImage>
#include <QTime>
#include <QQuickItem>
#include "AVRecordPlay.hpp"

#include "media/MediaCore.hpp"

class QPainter;
class AVRecordPainted : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QImage mImage READ getImage WRITE setImage)
    Q_PROPERTY(QString fileUrl READ getFileUrl WRITE setFileUrl)
    Q_PROPERTY(int mWidth READ getWidth WRITE setWidth)
    Q_PROPERTY(int mHeight READ getHeight WRITE setHeight)


public:
    AVRecordPainted(QQuickItem *parent = 0);
    virtual ~AVRecordPainted();

    QImage mImage;
    int mWidth;
    int mHeight;
    float mZoom;
    QString fileUrl = "/dev/video0";

    AVRecordPlay *avrp;

    MediaState *ms;

    virtual void paint(QPainter *painter);

    QImage getImage() const;
    void setImage(QImage img);

    QString getFileUrl() const;
    void setFileUrl(QString url);

    int getWidth() const;
    void setWidth(int w);

    int getHeight() const;
    void setHeight(int h);


signals:
    void requestUpdate();

public slots:
    void procUpdate();
    void exit();
    void record();
    void photo();
};

#endif // AVRECORDPAINTED_H
