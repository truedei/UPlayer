#ifndef AVRECORDPLAY_H
#define AVRECORDPLAY_H

#include "string"
using namespace std;

//extern "C" {
//#include <libavutil/frame.h>
//#include <libavutil/imgutils.h>
//#include <libswscale/swscale.h>
//#include <libavformat/avformat.h>
//#include <libavcodec/avcodec.h>
//#include "libavdevice/avdevice.h"
//#include "libswresample/swresample.h"
//}


class AVRecordPlay
{
private:
    string camDevicesPath; // /dev/video0

public:
    AVRecordPlay(void *avRecordPainted);
    ~AVRecordPlay();

    static void* playVideoThr(void *uVO);


    string getCamDevicesPath() const;
    void setCamDevicesPath(const string &newCamDevicesPath);
};

#endif // AVRECORDPLAY_H
