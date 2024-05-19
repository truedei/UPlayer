#include "AVRecordPainted.hpp"
#include <QPainter>
#include <QImage>
#include <unistd.h>
#include <thread>

AVRecordPainted::AVRecordPainted(QQuickItem *parent)
    :QQuickPaintedItem(parent)
    ,mWidth(0), mHeight(0),mZoom(1.0f)
{



    QObject::connect(this, &AVRecordPainted::requestUpdate,
                     this, &AVRecordPainted::procUpdate);


    //提前加载摄像头设备数据


    //进来就直接显示摄像头画面
    avrp = new AVRecordPlay((void*)this);
}

AVRecordPainted::~AVRecordPainted()
{

}

void AVRecordPainted::paint(QPainter *painter)
{
    if (painter == nullptr)
    {
        return;
    }

    int status = 1;

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
}


QString AVRecordPainted::getFileUrl() const
{
    return fileUrl;
}


void AVRecordPainted::setFileUrl(QString url)
{
//    fileUrl = url;
//    uPlayer->setStrBackgroundPath(url);


    printf("url=%s \n", url.toStdString().c_str());

//    std::thread play = std::thread(playVideoThr,(AVRecordPainted*)this);
//    play.detach();
}

int AVRecordPainted::getWidth() const
{
    return mWidth;
}

void AVRecordPainted::setWidth(int w)
{
    mWidth = w;
}

int AVRecordPainted::getHeight() const
{
    return mHeight;
}

void AVRecordPainted::setHeight(int h)
{
    mHeight = h;
}

QImage AVRecordPainted::getImage() const
{
    return mImage;
}

void AVRecordPainted::setImage(QImage img)
{
    mImage = img;
    //刷新显示
    emit requestUpdate();
}

void AVRecordPainted::procUpdate()
{
    update();
}

void AVRecordPainted::exit()
{
    qDebug() <<__FUNCTION__ ;
    //恢复当前播放的媒体文件名字
    if(this->ms == NULL)
    {
        return ;
    }
    this->ms->exit_request = 1;
}

static bool record_flag = false;

void AVRecordPainted::record()
{

    if(this->ms == NULL)
    {
        return ;
    }

    // C++ 使用匿名函数作为线程函数
    std::thread thread2([&]() {
        if(!record_flag)
        {
            qDebug() <<__FUNCTION__ << "---> start record";

            //init AVRecord
            this->ms->avRecord = new AVRecord();

            VideoEncObj venc;
            venc.bitrate = this->ms->decode_v_ctx->bit_rate;
            venc.codec_id = this->ms->decode_v_ctx->codec_id;
            venc.width = this->ms->decode_v_ctx->width;
            venc.height = this->ms->decode_v_ctx->height;
            venc.profile_id = 3;

            this->ms->avRecord->setVencObj(&venc);

            string path = "";

            this->ms->avRecord->setRecordFile("/data/project/Uplayer/UplayerCodes/videotest/test.mp4");

            this->ms->avRecord->InitAVRecorder();

            sleep(2);
            this->ms->model_type = MODEL_TYPE_RECORD;

            record_flag = true;
        }else{
            qDebug() <<__FUNCTION__ << "---> close record";

            this->ms->model_type = MODEL_TYPE_NULL;
            sleep(1);
            this->ms->avRecord->CloseAVRecorder();
            record_flag = false;
        }
    });

    thread2.detach();
}


void AVRecordPainted::photo()
{
    qDebug() <<__FUNCTION__ ;

    if(this->ms == NULL)
    {
        return ;
    }





    this->ms->model_type = MODEL_TYPE_PHOTO;
}





