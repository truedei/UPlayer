#ifndef MEDIACORE_H_
#define MEDIACORE_H_

#include "string"
#include "SDL.h"
#include "SDL_audio.h"
#include "list"
#include "thread"
#include "map"
#include "queue"
#include "ffmpeg_common.h"
#include "MediaRecord.hpp"

//  audio sample rates map from FFMPEG to SDL (only non planar)
static std::map<int,int> AUDIO_FORMAT_MAP = {
        // AV_SAMPLE_FMT_NONE = -1,
        {AV_SAMPLE_FMT_U8,  AUDIO_U8    },
        {AV_SAMPLE_FMT_S16, AUDIO_S16SYS},
        {AV_SAMPLE_FMT_S32, AUDIO_S32SYS},
        {AV_SAMPLE_FMT_FLT, AUDIO_F32SYS}
};


typedef struct PacketQueue {
    std::list<AVPacket> pkt_list;
    int nb_packets;
    int size;
} PacketQueue;


enum MEDIA_TYPE{
    MEDIA_TYPE_MEDIA_PLAY,
    MEDIA_TYPE_RECORD_PLAY,
};

enum MODEL_TYPE{
    MODEL_TYPE_NULL,
    MODEL_TYPE_RECORD,
    MODEL_TYPE_PHOTO,
};

typedef struct MediaState{
    std::string filename; //文件名

    int exit_request;//1:exit

    //暂停状态：0:未暂停，1：暂停中
    int paused; //1:暂停，:0播放
    int last_paused;    //存储last paused status (“暂停”或“播放”状态)
    int seek_req; //标识一次seek请求
    int seek_time; //seek time === totalDuration[0,N]

    // media type:media play,record play
    MEDIA_TYPE media_type;

    MODEL_TYPE model_type;

    const AVInputFormat *iformat;   //指向demuxer
    AVFormatContext *ic; //iformat的上下文

    //总时长
    int totalDuration;
    std::string totalDurationStr;
    char currPlayTime[30];


    int video_stream_index;
    AVStream *video_st;
    PacketQueue videoq;//video队列

    int audio_stream_index;
    AVStream *audio_st;
    PacketQueue audioq;//audio队列

    std::thread read_thr;//read thread
    std::thread write_thr;//write thread

    //data queue
    std::queue<AVPacket> dataQueue;


    const AVCodec *codec_video = NULL;
    const AVCodec *codec_audio = NULL;
    //decode
    AVCodecContext *decode_v_ctx = NULL;
    AVCodecContext *decode_a_ctx = NULL;
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

    void* uVideoOutput;

    AVRecord *avRecord;

} MediaState;


//open stream
MediaState *MEDIA_stream_open(MEDIA_TYPE media_t,std::string filename, void *uVideoOutput);

void MEDIA_stream_exit(MediaState *ms);

#endif // MEDIA_H
