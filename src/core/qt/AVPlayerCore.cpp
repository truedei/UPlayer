#include "AVPlayerCore.hpp"
#include <QPainter>
#include <QImage>
#include <unistd.h>
#include "common.h"
#include "iostream"
#include "utils/configutils.h"

static ConfigUtils config;

UVideoOutput::UVideoOutput(QQuickItem *parent)
    :QQuickPaintedItem(parent)
    ,mWidth(0), mHeight(0),mZoom(1.0f)
{
    qDebug()<<__FUNCTION__;

    uPlayer = new UPlayer();

    update();

    QObject::connect(this, &UVideoOutput::requestUpdate,
                     this, &UVideoOutput::procUpdate);



}

UVideoOutput::~UVideoOutput()
{

}

void UVideoOutput::zoomDraw(float z)
{
    mZoom = z;
    this->update();
}

void UVideoOutput::updateBackgroupImage(QString fileUrl)
{
    status = 0;
    setFileUrl(fileUrl);
    this->update();
}

void* UVideoOutput::playVideoThr(std::string filepath, UVideoOutput *uVO){
    UVideoOutput *uVideoOutput = uVO;

    qDebug() << "filepath=" << filepath.c_str() ;

    uVideoOutput->ms = MEDIA_stream_open(MEDIA_TYPE_MEDIA_PLAY,filepath, uVideoOutput);

    if(uVideoOutput->ms){
        uVideoOutput->ms->sdl_volume = config.getPlayerVolume();
    }

    return 0;
}

void UVideoOutput::urlPass(QString url)
{
    this->status = 1;

    //通知UI，更新正在播放的视频名字
    this->setCurrAVTitle(url);

    this->play_thr = std::thread(playVideoThr,url.toStdString(),(UVideoOutput*)this);
    this->play_thr.detach();
}

void UVideoOutput::stopPlay()
{
    qDebug() <<__FUNCTION__ ;
    //恢复当前播放的媒体文件名字
    if(this->ms == NULL)
    {
        return ;
    }
    this->ms->exit_request = 1;

    //修改播放状态
    this->status = 0;
    this->setCurrAVTitle("");
    this->setCurrPlayTime(0);
    this->setCurrPlayTimeStr("00:00:00");
}


void UVideoOutput::updateVolume(int volume)
{
    config.setPlayerVolume(volume);

    if(this->ms)
    {
        ms->sdl_volume = volume;
    }
}

int UVideoOutput::getVolume()
{
    qDebug() <<__FUNCTION__ ;
    if(this->ms)
    {
        return ms->sdl_volume;
    }

    return 0;
}

void UVideoOutput::playSeek(int seek_time)
{
    if(this->ms)
    {
        ms->seek_req = 1;
        ms->seek_time = seek_time;
    }
}

void UVideoOutput::playUp()
{
    if(this->ms)
    {
        ms->seek_req = 2;
        ms->seek_time = 5;
    }
}

void UVideoOutput::playDown()
{
    if(this->ms)
    {
        ms->seek_req = 3;
        ms->seek_time = -5;
    }
}


int UVideoOutput::startPlay()
{
    if(this->ms)
    {
        this->ms->paused = ~(this->ms->paused);
        if(this->ms->paused == 0)
        {
            return 1;
        }
        return 2;
    }
    return 0;
}

int UVideoOutput::getPlayState(){
    if(this->ms)
    {
        if(this->ms->paused == 0)
        {
            return 1;
        } else if(this->ms->paused == 1){
            return 2;
        }
    }
    return 0;
}

void UVideoOutput::paint(QPainter *painter)
{
    if (painter == nullptr)
    {
        return;
    }

    //0:设置背景,1:播放视频
    if(status == 0)
    {
        if(!fileUrl.isEmpty())
        {
            if(!mImage.load(fileUrl))
            {
                qDebug("load image file failed!");
                return;
            }
        }
    }
    else if(status == 1){
        if(mImage.isNull() )
        {
            qDebug("load image file failed!");
            return;
        }
    }

    // 自适应
    QPixmap pixmap = QPixmap::fromImage(mImage);

    double widgetWidth = this->width();
    double widgetHeight = this->height();
//    qDebug("widgetWidth=%f,widgetHeight=%f \n", widgetWidth,widgetHeight);

    painter->drawPixmap(0,0,widgetWidth,widgetHeight,pixmap);

    //zoom
    //    QImage tempImage = mImage.scaled(mWidth*mZoom,mHeight*mZoom,Qt::KeepAspectRatio);
    //    painter->setRenderHint(QPainter::Antialiasing, true);
    //    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
    //    painter->setRenderHint(QPainter::SmoothPixmapTransform, true );
    //    painter->drawImage(0,0, tempImage);
}

QImage UVideoOutput::getImage() const
{
    return mImage;
}

void UVideoOutput::setImage(QImage img)
{
    mImage = img;
    //刷新显示
    emit requestUpdate();
}

QString UVideoOutput::getFileUrl() const
{
    return fileUrl;
}

void UVideoOutput::setFileUrl(QString url)
{
    fileUrl = url;
    uPlayer->setStrBackgroundPath(url);
}

int UVideoOutput::getWidth() const
{
    return mWidth;
}

void UVideoOutput::setWidth(int w)
{
    mWidth = w;
}

int UVideoOutput::getHeight() const
{
    return mHeight;
}

void UVideoOutput::setHeight(int h)
{
    mHeight = h;
}


int UVideoOutput::getShowType() const{
    return mShowType;
}

void UVideoOutput::setShowType(int st)
{
    this->mShowType = st;
    qDebug()<<__FUNCTION__ << "showtype=" << this->mShowType;
}

void UVideoOutput::procUpdate()
{
    update();
}


void UVideoOutput::setTotalDuration(int totalDuration)
{

    if(this->_totalDuration != totalDuration)
    {
        this->_totalDuration = totalDuration;
        emit totalDurationChanged();
    }
}

int UVideoOutput::getTotalDuration()
{
    return this->_totalDuration;
}

void UVideoOutput::setTotalDurationStr(QString totalDurationStr)
{
    this->_totalDurationStr = totalDurationStr;
    emit totalDurationStrChanged();
}

QString UVideoOutput::getTotalDurationStr()
{
    return this->_totalDurationStr;
}

void UVideoOutput::setCurrPlayTime(int currPlayTime)
{
    if(currPlayTime > this->_currPlayTime)
    {
        this->_currPlayTime = currPlayTime;
//        emit currPlayTimeChanged();
    }

    if(currPlayTime == 0){
        this->_currPlayTime = currPlayTime;
        emit currPlayTimeChanged();
    }
}

int UVideoOutput::getCurrPlayTime()
{
    return this->_currPlayTime;
}


void UVideoOutput::setCurrPlayTimeStr(QString currPlayTimeStr)
{
    if(this->_currPlayTimeStr != currPlayTimeStr)
    {
        this->_currPlayTimeStr = currPlayTimeStr;
        emit currPlayTimeStrChanged();
        emit currPlayTimeChanged();
    }
}

QString UVideoOutput::getCurrPlayTimeStr()
{
    return this->_currPlayTimeStr;
}

void UVideoOutput::setCurrAVTitle(QString url)
{
    //通知UI，更新正在播放的视频名字
    QStringList tmpList = url.split("/");
    this->_currAVTitle = tmpList[tmpList.length() - 1];
    emit updateCurrAVTitleChanged();
}

QString UVideoOutput::getCurrAVTitle()
{
    return this->_currAVTitle;
}





