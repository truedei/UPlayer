#include "MediaCamera.hpp"
#include "MediaCore.hpp"
#include "qt/AVRecordPainted.hpp"
#include <iostream>
#include "MediaFFmpeg.hpp"

static MediaFFmpeg md;

MediaCamera::MediaCamera()
{

}


/************************************************************************************************************/
//                                      Open  Camera
//************************************************************************************************************/
//使用SDL播放音频
static unsigned int audioLen = 0;
static unsigned char *audioChunk = NULL;
static unsigned char *audioPos = NULL;

static void fill_audio(void * udata, Uint8 * stream, int len)
{
    SDL_memset(stream, 0, len);

    if(audioLen == 0)
        return;

    len = (len>audioLen ? audioLen : len);

    MediaState *ms = (MediaState*)udata;

    SDL_MixAudio(stream, audioPos, len, ms->sdl_volume);

    audioPos += len;
    audioLen -= len;
}

void decode_av_pkt(MediaState *ms)
{
    AVRecordPainted* avrp = (AVRecordPainted*)(ms->uVideoOutput);

    SwsContext *pImgConvertCtx;
    pImgConvertCtx = sws_getContext(ms->decode_v_ctx->width, ms->decode_v_ctx->height,
            ms->decode_v_ctx->pix_fmt, ms->decode_v_ctx->width, ms->decode_v_ctx->height,
            AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    // 分配pkt和frame
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    int ret = 0;
    // 为frame分配buffer
    frame->format = AV_PIX_FMT_YUV420P;
    frame->width  = ms->decode_v_ctx->width;
    frame->height = ms->decode_v_ctx->height;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    //video
    if (ms->packet->stream_index == ms->video_stream_index){

        //解码一帧视频数据
        ret = avcodec_send_packet(ms->decode_v_ctx, ms->packet);

        if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            std::cout << "avcodec_send_packet: " << ret ;
            goto end;
        }
        while (avcodec_receive_frame(ms->decode_v_ctx, ms->pFrame_video) == 0) {


            //QML show 获取到 pFrame 解码数据，在这里进行格式转换，然后进行渲染
            sws_scale(ms->img_convert_ctx,
                      (const unsigned char* const*)ms->pFrame_video->data,
                      ms->pFrame_video->linesize,
                      0,
                      ms->decode_v_ctx->height,
                      ms->pFrameRGB_video->data,
                      ms->pFrameRGB_video->linesize);

            QImage img((uchar*)ms->pFrameRGB_video->data[0],ms->pFrame_video->width,ms->pFrame_video->height,QImage::Format_RGB32);

            //播放每一帧视频
            avrp->setImage(img);

            if(ms->model_type == MODEL_TYPE_RECORD)
            {

                sws_scale(pImgConvertCtx,
                        (const unsigned char* const*) ms->pFrame_video->data,
                        ms->pFrame_video->linesize,
                          0,
                          ms->decode_v_ctx->height,
                          frame->data,
                          frame->linesize);

                frame->format = ms->decode_v_ctx->pix_fmt;
                frame->width = ms->decode_v_ctx->width;
                frame->height = ms->decode_v_ctx->height;

//              printf("ms->decode_v_ctx->width=%d ,ms->decode_v_ctx->height=%d\n",ms->decode_v_ctx->width, ms->decode_v_ctx->height);
                ms->avRecord->WriteData(frame);
            }
            av_frame_unref(ms->pFrame_video);
        }
    }

    //audio
    else if(ms->packet->stream_index == ms->audio_stream_index){
        ret = avcodec_send_packet(ms->decode_a_ctx, ms->packet); // 送一帧到解码器

        while(ret >= 0)
        {
            ret = avcodec_receive_frame(ms->decode_a_ctx, ms->pFrame_audio);  // 从解码器取得解码后的数据

            if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            }
            else if(ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
                continue;
            }

            ret = swr_convert(ms->audio_convert_ctx,
                              &ms->out_buffer_audio,
                              MAX_AUDIO_FRAME_SIZE,
                              (const uint8_t **)ms->pFrame_audio->data,
                              ms->pFrame_audio->nb_samples);
            if(ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while converting\n");
                goto end;
            }

            while(audioLen > 0)
                SDL_Delay(2);

            audioChunk = (unsigned char *)(ms->out_buffer_audio);
            audioPos = audioChunk;
            audioLen = ms->out_buffer_audio_size;

            av_frame_unref(ms->pFrame_audio);
        }
    }

end:

    av_frame_free(&frame);

    return;
}



void record_play_read_thread (void*arg) {
    MediaState *ms = (MediaState*)arg;
    for (;;) {
        //检测是否退出
        if(ms->exit_request)
        {
            cout << "exit media_play_read_thread()" <<endl;
            goto end;
        }

        //read media data
        int ret = -1;
        while (((ret=av_read_frame(ms->ic, ms->packet)) >= 0)
               && ms->exit_request == 0){
            decode_av_pkt(ms);
        }

        if(ret < 0)
        {
            goto end;
        }
    }

end:

    MEDIA_stream_exit(ms);
    return ;
}



static std::queue<AVPacket> dataQueue;

int MediaCamera::InitCamera(void *_ms){
    if(!_ms){
        return -1;
    }

    MediaState *ms = (MediaState *)_ms;

    AVRecordPainted*  avrp = (AVRecordPainted*)(ms->uVideoOutput);

    AVFormatContext *ic = NULL;

    int ret = -1;

    ret = md.DecoderInit(ms->filename,
                   &ic,
                   &ms->video_stream_index,
                   &ms->audio_stream_index,
                   &ms->codec_video,
                   &ms->codec_audio,
                   &ms->decode_v_ctx,
                   &ms->decode_a_ctx);

    //get media info
    MediaInfo mi;
    //init 解码器
    if(ms->video_stream_index >= 0 )
    {
        ms->pFrame_video = av_frame_alloc();
        ms->packet = av_packet_alloc();
        md.DecoderGetMediaInfo(&mi, ms->decode_v_ctx, ic->streams[ms->video_stream_index]);
        md.SWSInit2((void**)&ms->img_convert_ctx,
                   &ms->out_buffer_video,
                   &ms->pFrameRGB_video,
                   ms->decode_v_ctx,
                   ic->streams[ms->video_stream_index]);
    }

    if(ms->audio_stream_index >= 0){
        md.DecoderGetMediaInfo(&mi, ms->decode_a_ctx, ic->streams[ms->audio_stream_index]);
        md.SWSInit2((void**)&ms->audio_convert_ctx,
                   &ms->out_buffer_audio,
                   &ms->pFrame_audio,
                   ms->decode_a_ctx,
                   ic->streams[ms->audio_stream_index]);

        ms->out_buffer_audio_size = mi.audio_buffer_size;
        //   SDL
        if(SDL_Init(SDL_INIT_AUDIO)) {
            SDL_Log("init audio subsysytem failed.");
            return -1;
        }

        SDL_AudioSpec wantSpec;
        wantSpec.freq = mi.sample_rate;
        // 和SwrContext的音频重采样参数保持一致
        wantSpec.format = AUDIO_FORMAT_MAP[mi.sample_fmt];
        wantSpec.channels = mi.channels;
        wantSpec.silence = 0;
        wantSpec.samples = mi.nb_samples;
        wantSpec.callback = fill_audio;
//        wantSpec.userdata = ms->decode_a_ctx;
        wantSpec.userdata = (void*)ms;

        if(SDL_OpenAudio(&wantSpec, NULL) < 0) {
            printf("can not open SDL!\n");
            return -1;
        }

        SDL_PauseAudio(0);
    }

    ms->ic = ic;
    ms->read_thr = std::thread(record_play_read_thread,ms);
    ms->read_thr.detach(); //分离线程，不阻塞
    return 0;
}

