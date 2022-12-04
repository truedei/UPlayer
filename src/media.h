#ifndef MEDIA_H_HH_
#define MEDIA_H_HH_

#include "string"
#include "SDL.h"
#include "SDL_audio.h"
#include "list"
#include "thread"
#include "map"

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include "libavdevice/avdevice.h"
#include "libswresample/swresample.h"
}


//  audio sample rates map from FFMPEG to SDL (only non planar)
static std::map<int,int> AUDIO_FORMAT_MAP = {
        // AV_SAMPLE_FMT_NONE = -1,
        {AV_SAMPLE_FMT_U8,  AUDIO_U8    },
        {AV_SAMPLE_FMT_S16, AUDIO_S16SYS},
        {AV_SAMPLE_FMT_S32, AUDIO_S32SYS},
        {AV_SAMPLE_FMT_FLT, AUDIO_F32SYS}
};

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio    //48000 * (32/8)



typedef struct PacketQueue {
    std::list<AVPacket> pkt_list;
    int nb_packets;
    int size;
} PacketQueue;




typedef struct MediaState{
    std::string filename; //文件名

    int exit_request;//1:exit

    //暂停状态：0:未暂停，1：暂停中
    int paused; //1:暂停，:0播放
    int last_paused;    //存储last paused status (“暂停”或“播放”状态)
    int seek_req; //标识一次seek请求
    int seek_time; //seek time === totalDuration[0,N]

    const AVInputFormat *iformat;   //指向demuxer
    AVFormatContext *ic; //iformat的上下文

    //总时长
    int totalDuration;
    char totalDurationStr[30];
    char currPlayTime[30];


    int video_stream_index;
    AVStream *video_st;
    PacketQueue videoq;//video队列

    int audio_stream_index;
    AVStream *audio_st;
    PacketQueue audioq;//audio队列

    std::thread read_thr;//read thread


    const AVCodec *codec_video = NULL;
    const AVCodec *codec_audio = NULL;
    AVCodecContext *pCtx_video = NULL;
    AVCodecContext *pCtx_audio = NULL;
    AVFrame    *pFrame_video = NULL;
    AVFrame    *pFrame_audio = NULL;
    AVFrame    *pFrameRGB_video = NULL;
    AVPacket *packet  = NULL;
    unsigned char *out_buffer_video ;
    unsigned char *out_buffer_audio ;
    int out_buffer_audio_size;
    struct SwsContext *img_convert_ctx = NULL;
    struct SwrContext *audio_convert_ctx  = NULL;

    unsigned int sdl_volume = SDL_MIX_MAXVOLUME;
    int out_sample_rate;  //采样率
    int out_channels;     //通道数
    int out_nb_samples;
    uint64_t out_chn_layout;
    enum AVSampleFormat out_sample_fmt;


    void* uVideoOutput;

} MediaState;


//open stream
MediaState *MEDIA_stream_open(std::string filename, void *uVideoOutput);


#endif // MEDIA_H
