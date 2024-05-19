#include "MediaFFmpeg.hpp"
#include <iostream>
#include <stdio.h>

MediaFFmpeg::MediaFFmpeg()
{

}

int MediaFFmpeg::DecoderInit(std::string url,
                              AVFormatContext **ictx,
                              int *v_index,
                              int *a_index,
                              const AVCodec **_v_codec,
                              const AVCodec **_a_codec,
                              AVCodecContext **_v_decode_ctx,
                              AVCodecContext **_a_decode_ctx){

    AVFormatContext *ic = NULL;
    const AVCodec *v_codec = NULL;
    const AVCodec *a_codec = NULL;
    AVCodecContext *v_decode_ctx = NULL;
    AVCodecContext *a_decode_ctx = NULL;

    //init devices
    avdevice_register_all();

    int ret = 0;
    //1.创建解复用上下文
    ic = avformat_alloc_context();
    if (!ic) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
        return -1;
    }
    *ictx = ic;

    //2.open input url
    ret = avformat_open_input(&ic, url.c_str(), NULL, NULL);
    if(ret < 0){
        av_log(NULL, AV_LOG_FATAL, "avformat_open_input error \n");
        return -1;
    }

    ret = avformat_find_stream_info(ic, NULL);
    if(ret < 0){
        av_log(NULL, AV_LOG_FATAL, "avformat_find_stream_info error \n");
        return -1;
    }

    av_dump_format(ic, 0, url.c_str(), 0);

    *v_index = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1,0,&v_codec,0);
    *a_index = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1,0,&a_codec,0);

    if(*v_index >= 0){
        //alloc video decode ctx
        v_decode_ctx = avcodec_alloc_context3(v_codec);
        if(!v_decode_ctx){
            av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
            return -1;
        }

        AVStream *v_stream = ic->streams[*v_index];

        //copy video parameters
        ret = avcodec_parameters_to_context(v_decode_ctx, v_stream->codecpar);
        if(ret < 0){
            av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
            return -1;
        }

        av_opt_set(v_decode_ctx->priv_data, "tune", "zerolatency", 0);

        ret = avcodec_open2(v_decode_ctx, v_codec, NULL);
        if(ret < 0){
            av_log(NULL, AV_LOG_FATAL, "avcodec_open2 error \n");
            return -1;
        }

        *_v_codec = v_codec;
        *_v_decode_ctx = v_decode_ctx;
    }

    if(*a_index >= 0){
        //alloc audio decode ctx
        a_decode_ctx = avcodec_alloc_context3(a_codec);
        if(!a_decode_ctx){
            av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
            return -1;
        }

        AVStream *a_stream = ic->streams[*a_index];
        //copy audio parameters
        ret = avcodec_parameters_to_context(a_decode_ctx, a_stream->codecpar);
        if(ret < 0){
            av_log(NULL, AV_LOG_FATAL, "avcodec_alloc_context3 error \n");
            return -1;
        }


        ret = avcodec_open2(a_decode_ctx, a_codec, NULL);
        if(ret < 0){
            av_log(NULL, AV_LOG_FATAL, "avcodec_open2 error \n");
            return -1;
        }

        *_a_codec = a_codec;
        *_a_decode_ctx = a_decode_ctx;
    }
    return 0;
}

int MediaFFmpeg::DecoderGetMediaInfo(MediaInfo *mi,AVCodecContext *decodec_ctx,AVStream *stream){
    if(!mi || !stream || !decodec_ctx){
        return -1;

    }

    //video
    if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        //获取视频总时长
        if(AV_NOPTS_VALUE != stream->duration)
        {
            mi->duration =  stream->duration * av_q2d(stream->time_base);
            std::cout << "video_time : " <<
                    (mi->duration / 3600)<< ":" <<
                    (mi->duration % 3600) / 60<< ":" <<
                    (mi->duration % 60) << std::endl;

            char formatStr[128] = {0,};
                    sprintf(formatStr, "%02d:%02d:%02d",
                    (mi->duration / 3600),
                    ((mi->duration % 3600) / 60),
                    (mi->duration % 60));
            mi->durationFormatStr = formatStr;
        }
        else{
            printf("audio duration unknown ! \n");
        }

        mi->width = stream->codecpar->width;
        mi->height = stream->codecpar->height;
    }
    //audio
    else if(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
        mi->sample_fmt                  =  AV_SAMPLE_FMT_S16;
        mi->sample_rate                 =   decodec_ctx->sample_rate;  //采样率/*48000; */
        mi->channels                    =   decodec_ctx->channels;     //通道数/*1;  */
        mi->nb_samples                  =   decodec_ctx->frame_size;/*1024;  */
        mi->audio_buffer_size           =   av_samples_get_buffer_size(NULL, mi->channels, mi->nb_samples, (enum AVSampleFormat)mi->sample_fmt, 1);   //输出buff
    }

    return 0;
}


/*   sws  */
int MediaFFmpeg::SWSInit(struct SwsContext **swsCtx,
                         struct SwrContext **swrCtx,
            unsigned char **out_buffer_video,
            unsigned char **out_buffer_audio,
            AVFrame **outFrame,
            AVCodecContext *decode_ctx,
            AVStream *stream){


    if(!decode_ctx || !stream){
        return -1;
    }

    if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        *out_buffer_video = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, decode_ctx->width, decode_ctx->height, 1));

        AVFrame *oFrame = av_frame_alloc();
        av_image_fill_arrays(
                    oFrame->data,
                    oFrame->linesize,
                    *out_buffer_video,
                    AV_PIX_FMT_RGB32,
                    decode_ctx->width,
                    decode_ctx->height,
                    1);


        //初始化img_convert_ctx结构 对H264做图像转换
        struct SwsContext *img_convert_ctx = sws_getContext(decode_ctx->width,
                                             decode_ctx->height,
                                             decode_ctx->pix_fmt,//AVPixelFormat
                                             decode_ctx->width,
                                             decode_ctx->height,
                                             AV_PIX_FMT_RGB32,
                                             SWS_BICUBIC,
                                             NULL, NULL, NULL);



        *outFrame = oFrame;
        *swsCtx = img_convert_ctx;
    }
    else if(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
        // 重采样contex
        enum AVSampleFormat out_sample_fmt  =   AV_SAMPLE_FMT_S16;   //声音格式  SDL仅支持部分音频格式
        int out_sample_rate                 =   decode_ctx->sample_rate;  //采样率/*48000; */
        int out_channels                    =   decode_ctx->channels;     //通道数/*1;  */
        int out_nb_samples                  =   decode_ctx->frame_size;/*1024;  */
        int out_buffer_audio_size           =   av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);   //输出buff
        unsigned char *_out_buffer_audio     =   (unsigned char *)av_malloc(MAX_AUDIO_FRAME_SIZE * out_channels);
        uint64_t out_chn_layout             =   av_get_default_channel_layout(decode_ctx->channels); //AV_CH_LAYOUT_STEREO;  //通道布局 输出双声道

        SwrContext *audio_convert_ctx = (struct SwrContext *)swr_alloc_set_opts(NULL,
                                                   out_chn_layout,                    /*out*/
                                                   out_sample_fmt,                    /*out*/
                                                   out_sample_rate,                   /*out*/
                                                   decode_ctx->channel_layout,           /*in*/
                                                   decode_ctx->sample_fmt,               /*in*/
                                                   decode_ctx->sample_rate,              /*in*/
                                                   0,
                                                   NULL);

        swr_init(audio_convert_ctx);

        *out_buffer_audio = _out_buffer_audio;
        *swrCtx =audio_convert_ctx;
    }

    return 0;
}


int MediaFFmpeg::SWSInit2(void **swCtx,
            unsigned char **out_buffer,
            AVFrame **outFrame,
            AVCodecContext *decode_ctx,
                          AVStream *stream){


    if(!decode_ctx || !stream){
        return -1;
    }

    if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
        *out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, decode_ctx->width, decode_ctx->height, 1));

        AVFrame *oFrame = av_frame_alloc();
        av_image_fill_arrays(
                    oFrame->data,
                    oFrame->linesize,
                    *out_buffer,
                    AV_PIX_FMT_RGB32,
                    decode_ctx->width,
                    decode_ctx->height,
                    1);


        //初始化img_convert_ctx结构 对H264做图像转换
        struct SwsContext *img_convert_ctx = sws_getContext(decode_ctx->width,
                                             decode_ctx->height,
                                             decode_ctx->pix_fmt,//AVPixelFormat
                                             decode_ctx->width,
                                             decode_ctx->height,
                                             AV_PIX_FMT_RGB32,
                                             SWS_BICUBIC,
                                             NULL, NULL, NULL);



        *outFrame = oFrame;
        *swCtx = (void*)img_convert_ctx;
    }
    else if(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
        AVFrame *oFrame = av_frame_alloc();

        // 重采样contex
        enum AVSampleFormat out_sample_fmt  =   AV_SAMPLE_FMT_S16;   //声音格式  SDL仅支持部分音频格式
        int out_sample_rate                 =   decode_ctx->sample_rate;  //采样率/*48000; */
        int out_channels                    =   decode_ctx->channels;     //通道数/*1;  */
        int out_nb_samples                  =   decode_ctx->frame_size;/*1024;  */
        int out_buffer_audio_size           =   av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);   //输出buff
        unsigned char *_out_buffer_audio     =   (unsigned char *)av_malloc(MAX_AUDIO_FRAME_SIZE * out_channels);
        uint64_t out_chn_layout             =   av_get_default_channel_layout(decode_ctx->channels); //AV_CH_LAYOUT_STEREO;  //通道布局 输出双声道


       SwrContext *audio_convert_ctx = (struct SwrContext *)swr_alloc_set_opts(NULL,
                                                   out_chn_layout,                    /*out*/
                                                   out_sample_fmt,                    /*out*/
                                                   out_sample_rate,                   /*out*/
                                                   decode_ctx->channel_layout,           /*in*/
                                                   decode_ctx->sample_fmt,               /*in*/
                                                   decode_ctx->sample_rate,              /*in*/
                                                   0,
                                                   NULL);

        swr_init(audio_convert_ctx);

        *outFrame = oFrame;
        *out_buffer = _out_buffer_audio;
        *swCtx = (void*)audio_convert_ctx;
    }


    return 0;
}


