#include "UVideoOutput.h"
#include <QPainter>
#include <QImage>

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
    update();
}



void Delay2(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

int UVideoOutput::playVideo2(const char *filepath,UVideoOutput *uVideoOutput)
{

    qDebug() << "filepath=" << filepath ;

    AVFormatContext    *pFormatCtx = NULL;
    int                i=0, videoindex=0;
    const AVCodec            *pCodec  = NULL;
    AVFrame    *pFrame , *pFrameRGB ;
    unsigned char *out_buffer ;
    AVPacket *packet  = NULL;
    int ret=0, got_picture = 0;
    struct SwsContext *img_convert_ctx ;
    AVCodecContext *pCtx = NULL;


    //1.创建封装格式上下文
    pFormatCtx = avformat_alloc_context();

    //初始化pFormatCtx结构
    //2.打开输入文件，解封装
    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0){
        printf("Couldn't open input stream.\n");
        return ret;
    }

    //3.获取音视频流数据信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0){
        printf("Couldn't find stream information.\n");
        return ret;
    }

    //4.获取音视频流索引
    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO), filepath);
        return ret;
    }

    videoindex = ret;

    if (videoindex == -1){
       printf("Didn't find a video stream.\n");
       return ret;
    }


    //5.获取解码器参数
    //获取视频流编码结构
//    pCodecCtx = pFormatCtx->streams[videoindex]->codec;

    AVStream *st = NULL;
    st = pFormatCtx->streams[videoindex];


    //6.根据 codec_id 获取解码器
    //查找解码器
    pCodec = avcodec_find_decoder(st->codecpar->codec_id);
    if (pCodec == NULL){
        printf("Codec not found.\n");
        return ret;
    }

    pCtx = avcodec_alloc_context3(NULL);
    if (!pCtx) {
      fprintf(stderr, "Could not allocate video codec context\n");
      exit(1);
    }

    /* Copy codec parameters from input stream to output codec context */
     if ((ret = avcodec_parameters_to_context(pCtx, st->codecpar)) < 0) {
         fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                 av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
         return ret;
     }

    //用于初始化pCodecCtx结构
    if (avcodec_open2(pCtx, pCodec, NULL) < 0){
        printf("Could not open codec.\n");
        return ret;
    }

    //9.创建存储编码数据和解码数据的结构体
    //创建帧结构，此函数仅分配基本结构空间，图像数据空间需通过av_malloc分配
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    //创建 AVPacket 存放编码数据
    packet = av_packet_alloc();

    //创建动态内存,创建存储图像数据的空间
    //av_image_get_buffer_size获取一帧图像需要的大小
    out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCtx->width, pCtx->height, 1));

    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, out_buffer,
        AV_PIX_FMT_RGB32, pCtx->width, pCtx->height, 1);

    //Output Info-----------------------------
    qDebug() << "--------------- File Information ----------------\n" ;
    //此函数打印输入或输出的详细信息
    av_dump_format(pFormatCtx, 0, filepath, 0);
    qDebug() << "-------------------------------------------------\n" ;



    //初始化img_convert_ctx结构
    img_convert_ctx = sws_getContext(pCtx->width,pCtx->height,
                                     pCtx->pix_fmt,//AVPixelFormat
                                     pCtx->width,
                                     pCtx->height,
                                     AV_PIX_FMT_RGB32,
                                     SWS_BICUBIC,
                                     NULL, NULL, NULL);


    //av_read_frame读取一帧未解码的数据
    while (av_read_frame(pFormatCtx, packet) >= 0 && uVideoOutput->status == 1){
       //如果是视频数据
       if (packet->stream_index == videoindex){
           //解码一帧视频数据
           ret = avcodec_send_packet(pCtx, packet);


           if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
              qDebug() << "avcodec_send_packet: " << ret ;
              break;
           }

           //int avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame);
           while (avcodec_receive_frame(pCtx, pFrame) == 0) {
               //获取到 m_Frame 解码数据，在这里进行格式转换，然后进行渲染，下一节介绍 ANativeWindow 渲染过程
//              qDebug() << "height:" << pFrame->height << " ; width:"<< pFrame->width << ";data=" <<  pFrame->data ;

              sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCtx->height,
                  pFrameRGB->data, pFrameRGB->linesize);

              QImage img((uchar*)pFrameRGB->data[0],pCtx->width,pCtx->height,QImage::Format_RGB32);


                //保存视频帧为图片
//              img.save("/home/zhenghui/桌面/aaa/a.png");

              //播放每一帧视频
              uVideoOutput->setImage(img);

              Delay2(10);

             qDebug() << "img height:" << pCtx->height << " ; img width:"<< pCtx->width;

           }

       }
       av_packet_unref(packet);
    }

end:
    if(img_convert_ctx != NULL)
    {
         sws_freeContext(img_convert_ctx);
    }

    if(pFrameRGB != NULL)
    {
         av_frame_free(&pFrameRGB);
    }

    if(pFrame != NULL)
    {
         av_frame_free(&pFrame);
    }

    if(pCtx != NULL)
    {
          avcodec_close(pCtx);
    }

    if(pFormatCtx != NULL)
    {
          avformat_close_input(&pFormatCtx);
    }


    return ret;
}

// 定义线程的 id 变量，多个变量使用数组
pthread_t recId;


struct Params
{
//    char * fileName;//参数1
    QString url;
    UVideoOutput *uVideoOutput;
};

void* UVideoOutput::playVideoTh(void* args){

    Params *param =  (Params*)args;

    qDebug() << "playVideo=====--->"<< param->url<<endl;
    UVideoOutput *uVideoOutput = param->uVideoOutput;

    playVideo2(param->url.toStdString().data(),uVideoOutput);


    return 0;
}

void UVideoOutput::urlPass(QString url)
{


    status = 1;
    qDebug() << "start video :" << url ;


    Params *param = new Params();
    param->url =url;
    param->uVideoOutput = this;

    qDebug() << "start video :" << url ;

    //QString to char*
//    char * fileUrl = (char*)url.toStdString().data();
    qDebug() << (url == nullptr) ;

    //如果没录制就启动线程
    qDebug() << "start video121 :" << param->url ;

    UVideoOutput XIV;

    int ret = pthread_create(&recId, NULL,playVideoTh, param);
    if (ret != 0)
    {
        qDebug() << "pthread_create error: error_code=" << ret ;
    }

}

void UVideoOutput::StopPlay()
{
    qDebug() <<__FUNCTION__ << endl;
}

void UVideoOutput::StartPlay()
{

    qDebug() <<__FUNCTION__ << endl;

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

    QImage tempImage = mImage.scaled(mWidth*mZoom,mHeight*mZoom,Qt::KeepAspectRatio);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true );
    painter->drawImage(0,0, tempImage);
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

void UVideoOutput::procUpdate()
{
    update();
}
