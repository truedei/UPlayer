#include "AVRecordPlay.hpp"

#include "AVRecordPainted.hpp"
#include <unistd.h>
#include <thread>



string AVRecordPlay::getCamDevicesPath() const
{
    return camDevicesPath;
}

void AVRecordPlay::setCamDevicesPath(const string &newCamDevicesPath)
{
    camDevicesPath = newCamDevicesPath;
}

AVRecordPlay::AVRecordPlay(void *avRecordPainted)
{
    //初次打开,就创建一个线程进行播放实时的摄像头画面
    std::thread play = std::thread(playVideoThr,(AVRecordPainted*)avRecordPainted);
    play.detach();
}

AVRecordPlay::~AVRecordPlay()
{

}


void* AVRecordPlay::playVideoThr(void *uVO){
    AVRecordPainted *uVideoOutput = (AVRecordPainted*)uVO;

    std::string filepath = uVideoOutput->getFileUrl().toStdString();

    uVideoOutput->ms = MEDIA_stream_open(MEDIA_TYPE_RECORD_PLAY,filepath, uVideoOutput);

//    for (int var = 0; var < 10; ++var)
//    for (int i = 1; i <= 5; i++) {
//        char fileurl[128] = {0,};
//        sprintf(fileurl,"/home/zhenghui/图片/壁纸/%d.jpg", i);
//        printf("fileurl=%s \n", fileurl);
//        QImage image;
//        image.load(fileurl);
//        if(image.isNull())
//        {
//            printf("image isnull \n");
//            continue;
//        }

//        uVideoOutput->setImage(image);
//        sleep(1);
//    }
    return 0;
}
