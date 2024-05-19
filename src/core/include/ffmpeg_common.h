#ifndef FFMPEG_COMMON_H
#define FFMPEG_COMMON_H

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include "libavdevice/avdevice.h"
#include "libswresample/swresample.h"
}

#endif // FFMPEG_COMMON_H
