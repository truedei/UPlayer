#include "media.h"
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include "iostream"
#include "unistd.h"
#include "map"
#include "QImage"

#include "UVideoOutput.h"

using namespace std;

std::mutex mtx;



//使用SDL播放音频
unsigned int audioLen = 0;
unsigned char *audioChunk = NULL;
unsigned char *audioPos = NULL;

void fill_audio(void * udata, Uint8 * stream, int len)
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



int av_read_play(AVFormatContext *s)
{
    if (s->iformat->read_play)
        return s->iformat->read_play(s);
    if (s->pb)
        return avio_pause(s->pb, 0);
    return AVERROR(ENOSYS);
}

int av_read_pause(AVFormatContext *s)
{
    if (s->iformat->read_pause)
        return s->iformat->read_pause(s);
    if (s->pb)
        return avio_pause(s->pb, 1);
    return AVERROR(ENOSYS);
}


void MEDIA_stream_exit(MediaState *ms)
{



    if(ms->img_convert_ctx != NULL)
    {
         sws_freeContext(ms->img_convert_ctx);
    }

    if(ms->pFrameRGB_video != NULL)
    {
         av_frame_free(&ms->pFrameRGB_video);
    }

    if(ms->pFrame_video != NULL)
    {
         av_frame_free(&ms->pFrame_video);
    }

    if(ms->pFrame_audio != NULL)
    {
         av_frame_free(&ms->pFrame_audio);
    }

    if(ms->ic)
    {
        avformat_close_input(&ms->ic);
    }

    // clear audio
    SDL_Quit();
    if(ms->audio_convert_ctx)
    {
      swr_free(&ms->audio_convert_ctx);
    }
    ms->video_stream_index = -1;
    ms->audio_stream_index = -1;
    ms->filename.clear();
    cout << "MEDIA_stream_exit success !" << endl;
}


void read_thread (void*arg) {
    MediaState *ms = (MediaState*)arg;

    //  mtx.lock();
    //  mtx.unlock();

    UVideoOutput* uVideoOutput = (UVideoOutput*)(ms->uVideoOutput);

    for (;;) {
        //检测是否退出
        if(ms->exit_request)
        {
            cout << "exit read_thread()" <<endl;
            goto end;
        }

        if(ms->last_paused != ms->paused)
        {
            ms->last_paused = ms->paused;
            if(ms->paused)
            {
                cout << "paused~~~~" <<endl;
                continue;
            }

            if (ms->paused)
                av_read_pause(ms->ic);
            else
                av_read_play(ms->ic);
        }

        if(ms->paused)
        {
            //10 ms
            SDL_Delay(10);
            continue;
        }

        // seek
        if(ms->seek_req == 1 || ms->seek_req == 2 || ms->seek_req == 3)
        {
            int incr = 0;

            int64_t seek_target = (int64_t)((ms->seek_time + uVideoOutput->_currPlayTime) * AV_TIME_BASE);

            //如果是拖动的进度条，就直接拿进度条的时间来计算
            if(ms->seek_req == 1)
            {
                seek_target = (int64_t)((ms->seek_time) * AV_TIME_BASE);
            }

            //stream_index->流索引，但是只有在 flags 包含 AVSEEK_FLAG_FRAME 的时候才是 设置某个流的读取位置。其他情况都只是把这个流的 time_base （时间基）作为参考。
            int stream_index = -1;
            //min_ts->跳转到的最小的时间，但是这个变量不一定是时间单位，也有可能是字节单位，也可能是帧数单位（第几帧）。
            int64_t min_ts = incr + 2;
            //ts->要跳转到的读取位置，单位同上。
            int64_t ts = seek_target;


            //max_ts->跳转到的最大的时间，单位同上，通常填 INT64_MAX 即可。
            int64_t max_ts = INT64_MAX;
            //flags->跳转的方式
            /*
                AVSEEK_FLAG_BYTE，按字节大小进行跳转。
                AVSEEK_FLAG_FRAME，按帧数大小进行跳转。
                AVSEEK_FLAG_ANY，可以跳转到非关键帧的读取位置，但是解码会出现马赛克。
                AVSEEK_FLAG_BACKWARD，往 ts 的后面找关键帧，默认是往 ts 的前面找关键帧。
            */
            int flags = 0;

            max_ts    = INT64_MAX;
            min_ts    = INT64_MIN;

//            cout << "ms->seek_time=" << ms->seek_time << endl;
//            cout << "stream_index=" << stream_index << endl;
//            cout << "min_ts=" << min_ts << endl;
//            cout << "ts=" << ts << endl;
//            cout << "max_ts=" << max_ts << endl;
//            cout << "flags=" << flags << endl;

            int ret = avformat_seek_file(ms->ic,  stream_index,  min_ts,  ts,  max_ts,  flags);
            if (ret < 0) {
              av_log(NULL, AV_LOG_ERROR,
                     "%s: error while seeking\n", ms->ic->url);
            }

            ms->seek_req = 0;
            ms->seek_time = 0;
        }


        //read media data
        int ret = -1;
        while (((ret=av_read_frame(ms->ic, ms->packet)) >= 0)
               && ms->exit_request == 0
               && ms->paused == 0
               && (ms->seek_req != 1 && ms->seek_req != 2 && ms->seek_req != 3)){

            //如果是视频数据
            if (ms->packet->stream_index == ms->video_stream_index){
                //解码一帧视频数据
                ret = avcodec_send_packet(ms->pCtx_video, ms->packet);

                if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    cout << "avcodec_send_packet: " << ret ;
                    goto end;
                }

                AVStream *stream=ms->ic->streams[ms->video_stream_index];
                //获取 curr play time 秒
                uint32_t second = ms->packet->pts * av_q2d(stream->time_base);
//                cout << "curr play time second : " << second <<endl;
//                cout << "curr play video_time : " <<
//                        (second / 3600)<< ":" <<
//                        (second % 3600) / 60<< ":" <<
//                        (second % 60) << endl;

                sprintf(ms->currPlayTime, "%02d:%02d:%02d",
                        (second / 3600),
                        ((second % 3600) / 60),
                        (second % 60));

                uVideoOutput->setCurrPlayTimeStr(ms->currPlayTime);

                uVideoOutput->setCurrPlayTime(second);

                while (avcodec_receive_frame(ms->pCtx_video, ms->pFrame_video) == 0) {
                    //获取到 pFrame 解码数据，在这里进行格式转换，然后进行渲染
                    sws_scale(ms->img_convert_ctx,
                              (const unsigned char* const*)ms->pFrame_video->data,
                              ms->pFrame_video->linesize,
                              0,
                              ms->pCtx_video->height,
                              ms->pFrameRGB_video->data,
                              ms->pFrameRGB_video->linesize);

                    QImage img((uchar*)ms->pFrameRGB_video->data[0],ms->pFrame_video->width,ms->pFrame_video->height,QImage::Format_RGB32);

                    //播放每一帧视频
                    uVideoOutput->setImage(img);

                    //SDL_Delay(10);

                    av_frame_unref(ms->pFrame_video);
                }
            }
            else if(ms->packet->stream_index == ms->audio_stream_index){
                ret = avcodec_send_packet(ms->pCtx_audio, ms->packet); // 送一帧到解码器

                while(ret >= 0)
                {
                    ret = avcodec_receive_frame(ms->pCtx_audio, ms->pFrame_audio);  // 从解码器取得解码后的数据

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
        }

        cout << "ret666=" <<ret<<endl;
        if(ret < 0)
        {
            goto end;
        }
    }

end:

    MEDIA_stream_exit(ms);
}


MediaState *MEDIA_stream_open(std::string filename,void *uVideoOutput)
{
    MediaState *ms;
    AVFormatContext *ic = NULL;

    int ret = -1;

    ms = (MediaState*)av_mallocz(sizeof (MediaState));
    if(!ms)
    {
        return NULL;
    }

    UVideoOutput* UVO = (UVideoOutput*)(uVideoOutput);
    ms->uVideoOutput = UVO;

    ms->video_stream_index = -1;
    ms->audio_stream_index = -1;
    ms->filename = filename;

    if (ms->filename.empty())
    {
        return NULL;
    }

    //1.创建解复用上下文
    ic = avformat_alloc_context();
    if (!ic) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
        return NULL;
    }

    //2.打开输入文件，解封装
    ret = avformat_open_input(&ic, ms->filename.c_str(), ms->iformat, NULL);
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_FATAL, "avformat_open_input error \n");
        return NULL;
    }

    //3.获取音视频流数据信息
    ret = avformat_find_stream_info(ic,NULL);
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_FATAL, "avformat_find_stream_info error \n");
        return NULL;
    }

    av_dump_format(ic, 0, ms->filename.c_str(), 0);



    //4.获取视频流索引
    ms->video_stream_index = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, ms->video_stream_index, -1, &ms->codec_video, 0);
    ms->audio_stream_index = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO,  ms->audio_stream_index, -1, &ms->codec_audio, 0);


    //init 解码器

    //获取文件总时长
    if(ms->video_stream_index >= 0 )
    {
        AVStream *video_stream = ic->streams[ms->video_stream_index];

        //获取视频总时长
        if(AV_NOPTS_VALUE != video_stream->duration)
        {
            ms->totalDuration =  video_stream->duration * av_q2d(video_stream->time_base);
            cout << "video_time : " <<
                    (ms->totalDuration / 3600)<< ":" <<
                    (ms->totalDuration % 3600) / 60<< ":" <<
                    (ms->totalDuration % 60) << endl;

            sprintf(ms->totalDurationStr, "%02d:%02d:%02d",
                    (ms->totalDuration / 3600),
                    ((ms->totalDuration % 3600) / 60),
                    (ms->totalDuration % 60));

            UVO->setTotalDuration(ms->totalDuration);
            UVO->setTotalDurationStr(ms->totalDurationStr);
        }
        else{
            printf("audio duration unknown ! \n");
        }


        //init 解码器
        ms->pCtx_video = avcodec_alloc_context3(ms->codec_video);
        if(!ms->pCtx_video)
        {
            cout << "Could not allocate video codec context" << endl;
            return NULL;
        }

        ret = avcodec_parameters_to_context(ms->pCtx_video, video_stream->codecpar);
        if(ret < 0)
        {
            cout << "avcodec_parameters_to_context error " << endl;
            return NULL;
        }

        ret = avcodec_open2(ms->pCtx_video, ms->codec_video , NULL);
        if(ret < 0)
        {
            cout << "avcodec_open2 error " << endl;
            return NULL;
        }

        ms->pFrame_video = av_frame_alloc();
        ms->pFrameRGB_video = av_frame_alloc();
        ms->packet = av_packet_alloc();
        ms->pFrame_audio = av_frame_alloc();

        ms->out_buffer_video = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32, ms->pCtx_video->width, ms->pCtx_video->height, 1));

        av_image_fill_arrays(
                    ms->pFrameRGB_video->data,
                    ms->pFrameRGB_video->linesize,
                    ms->out_buffer_video,
                    AV_PIX_FMT_RGB32,
                    ms->pCtx_video->width,
                    ms->pCtx_video->height,
                    1);

        //初始化img_convert_ctx结构 对H264做图像转换
        ms->img_convert_ctx = sws_getContext(ms->pCtx_video->width,
                                             ms->pCtx_video->height,
                                             ms->pCtx_video->pix_fmt,//AVPixelFormat
                                             ms->pCtx_video->width,
                                             ms->pCtx_video->height,
                                             AV_PIX_FMT_RGB32,
                                             SWS_BICUBIC,
                                             NULL, NULL, NULL);




    }

    if(ms->audio_stream_index >= 0)
    {
        ms->pCtx_audio = avcodec_alloc_context3(ms->codec_audio);

        AVCodecParameters *codecpar = ic->streams[ms->audio_stream_index]->codecpar;
        if((ret = avcodec_parameters_to_context(ms->pCtx_audio, codecpar)) < 0){
            avformat_close_input(&ic);
            avcodec_free_context(&ms->pCtx_audio);
            return NULL;
        }

        if((ret = avcodec_open2(ms->pCtx_audio, ms->codec_audio, NULL)) < 0) {
            avcodec_free_context(&ms->pCtx_audio);
            avformat_close_input(&ic);
            return NULL;
        }

        // 重采样contex
        ms->out_sample_fmt = AV_SAMPLE_FMT_S16;   //声音格式  SDL仅支持部分音频格式
        ms->out_sample_rate = /*48000; */ ms->pCtx_audio->sample_rate;  //采样率
        ms->out_channels =    /*1;  */    ms->pCtx_audio->channels;     //通道数
        ms->out_nb_samples = /*1024;  */  ms->pCtx_audio->frame_size;
        ms->out_buffer_audio_size = av_samples_get_buffer_size(NULL, ms->out_channels, ms->out_nb_samples, ms->out_sample_fmt, 1);   //输出buff
        ms->out_buffer_audio = (unsigned char *)av_malloc(MAX_AUDIO_FRAME_SIZE * ms->out_channels);
        ms->out_chn_layout =  av_get_default_channel_layout(ms->pCtx_audio->channels); //AV_CH_LAYOUT_STEREO;  //通道布局 输出双声道

        ms->audio_convert_ctx = swr_alloc_set_opts(NULL,
                                                   ms->out_chn_layout,                    /*out*/
                                                   ms->out_sample_fmt,                    /*out*/
                                                   ms->out_sample_rate,                   /*out*/
                                                   ms->pCtx_audio->channel_layout,           /*in*/
                                                   ms->pCtx_audio->sample_fmt,               /*in*/
                                                   ms->pCtx_audio->sample_rate,              /*in*/
                                                   0,
                                                   NULL);


        swr_init(ms->audio_convert_ctx);

        //   SDL
        if(SDL_Init(SDL_INIT_AUDIO)) {
            SDL_Log("init audio subsysytem failed.");
            return NULL;
        }

        SDL_AudioSpec wantSpec;
        wantSpec.freq = ms->out_sample_rate;
        // 和SwrContext的音频重采样参数保持一致
        wantSpec.format = AUDIO_FORMAT_MAP[ms->out_sample_fmt];
        wantSpec.channels = ms->out_channels;
        wantSpec.silence = 0;
        wantSpec.samples = ms->out_nb_samples;
        wantSpec.callback = fill_audio;
//        wantSpec.userdata = ms->pCtx_audio;
        wantSpec.userdata = (void*)ms;

        if(SDL_OpenAudio(&wantSpec, NULL) < 0) {
            printf("can not open SDL!\n");
            return NULL;
        }

        SDL_PauseAudio(0);
    }

    ms->ic = ic;

    //create read thread
    ms->read_thr = std::thread(read_thread,ms);
    ms->read_thr.detach(); //分离线程，不阻塞

fail:


    return ms;
}

