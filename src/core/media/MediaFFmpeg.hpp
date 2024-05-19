#ifndef MEDIAFFMPEG_H
#define MEDIAFFMPEG_H

#include "ffmpeg_common.h"
#include <string>

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio    //48000 * (32/8)


typedef struct MediaInfo
{
    int width;
    int height;
    int duration;
    std::string durationFormatStr;

    int sample_fmt;
    int sample_rate;  //采样率/*48000; */
    int channels;     //通道数/*1;  */
    int nb_samples;/*1024;  */
    int audio_buffer_size;
}MediaInfo;

class MediaFFmpeg
{
public:
    MediaFFmpeg();

    /*   Decoder  */
    int DecoderInit(std::string url,
                    AVFormatContext **ictx,
                    int *v_index,
                    int *a_index,
                    const AVCodec **v_codec,
                    const AVCodec **a_codec,
                    AVCodecContext **v_decode_ctx,
                    AVCodecContext **a_decode_ctx);

    int DecoderGetMediaInfo(MediaInfo *mi,AVCodecContext *decodec_ctx, AVStream *stream);


    int DecoderData();


    /*   Encoder  */


    /*   sws  */
    int SWSInit(struct SwsContext **swsCtx,
                struct SwrContext **swrCtx,
                unsigned char **out_buffer_video,
                unsigned char **out_buffer_audio,
                AVFrame **outFrame,
                AVCodecContext *v_decode_ctx,
                AVStream *stream);

    int SWSInit2(void **swCtx,
                unsigned char **out_buffer,
                AVFrame **outFrame,
                AVCodecContext *decode_ctx,
                AVStream *stream);

};

#endif // MEDIAFFMPEG_H
