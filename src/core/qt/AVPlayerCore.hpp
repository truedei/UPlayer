#ifndef AVPLAYERCORE_H
#define AVPLAYERCORE_H

#include <QObject>
#include <QImage>
#include <QQuickPaintedItem>
#include <QTime>
#include "AVPlayerUtils.hpp"
#include "media/MediaCore.hpp"

enum SHOW_TYPE{
    SHOW_TYPE_MEDIAPLAY,
    SHOW_TYPE_RECORD,
};


class QPainter;
class UVideoOutput : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QImage mImage READ getImage WRITE setImage)
    Q_PROPERTY(QString fileUrl READ getFileUrl WRITE setFileUrl)
    Q_PROPERTY(int mWidth READ getWidth WRITE setWidth)
    Q_PROPERTY(int mHeight READ getHeight WRITE setHeight)
    Q_PROPERTY(int mShowType READ getShowType WRITE setShowType)

    Q_PROPERTY(int totalDuration READ getTotalDuration WRITE setTotalDuration NOTIFY totalDurationChanged)
    Q_PROPERTY(QString currAVTitle READ getCurrAVTitle WRITE setCurrAVTitle NOTIFY updateCurrAVTitleChanged)

    Q_PROPERTY(QString totalDurationStr READ getTotalDurationStr WRITE setTotalDurationStr NOTIFY totalDurationStrChanged)
    Q_PROPERTY(int currPlayTime READ getCurrPlayTime WRITE setCurrPlayTime NOTIFY currPlayTimeChanged)
    Q_PROPERTY(QString currPlayTimeStr READ getCurrPlayTimeStr WRITE setCurrPlayTimeStr NOTIFY currPlayTimeStrChanged)



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

    int getShowType() const;
    void setShowType(int st);



    void setTotalDuration(int totalDuration);
    int getTotalDuration();

    void setTotalDurationStr(QString totalDurationStr);
    QString getTotalDurationStr();

    //当前当前播放时间
    void setCurrPlayTime(int currPlayTime);
    int getCurrPlayTime();

    void setCurrPlayTimeStr(QString currPlayTime);
    QString getCurrPlayTimeStr();

    void setCurrAVTitle(QString currAVTitle);
    QString getCurrAVTitle();


    Q_INVOKABLE void zoomDraw(float z = 1.0f);

    static void* playVideoThr(std::string filepath, UVideoOutput *uVO);
    UPlayer *uPlayer;

    MediaState *ms = NULL;

signals:
    void requestUpdate();
    void totalDurationChanged();
    void totalDurationStrChanged();
    void currPlayTimeChanged();
    void currPlayTimeStrChanged();
    void updateCurrAVTitleChanged();

public slots:
    void updateBackgroupImage(QString fileUrl);
    void urlPass(QString url);
    void stopPlay();
    int startPlay();
    int getPlayState();
    void procUpdate();
    void updateVolume(int volume);
    int getVolume();

    void playSeek(int seek_time);
    void playUp();
    void playDown();


private:


public:
   QImage mImage;
   QString fileUrl;
   int mWidth;
   int mHeight;
   float mZoom;

   //记录当前播放的视频文件的总时长
   int _totalDuration; // s
   QString _totalDurationStr; //xx:xx:xx

   //记录当前当前播放时间 xx:xx:xx
   int _currPlayTime = 0; // s
   QString _currPlayTimeStr; //xx:xx:xx

   //记录当前播放的视频名称
   QString _currAVTitle;

   //播放状态,0:设置背景,1:播放视频,-1:暂停播放
   int status=0;

    //plat thread
    std::thread play_thr;

    //type mode
    int mShowType;

};

#endif // AVPLAYERCORE_H
