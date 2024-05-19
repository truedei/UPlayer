#include "MediaFile.hpp"
#include "MediaCore.hpp"
#include "qt/AVRecordPlay.hpp"
#include "qt/AVPlayerCore.hpp"

#include "MediaFFmpeg.hpp"

#include <iostream>

static MediaFFmpeg md;

MediaFile::MediaFile()
{

}

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

void media_play_read_thread (void*arg) {
    MediaState *ms = (MediaState*)arg;
    UVideoOutput* uVideoOutput = (UVideoOutput*)(ms->uVideoOutput);

    for (;;) {
        //检测是否退出
        if(ms->exit_request)
        {
            std::cout << "exit media_play_read_thread()" << std::endl;
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
                ret = avcodec_send_packet(ms->decode_v_ctx, ms->packet);

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

                while (avcodec_receive_frame(ms->decode_v_ctx, ms->pFrame_video) == 0) {
                    //获取到 pFrame 解码数据，在这里进行格式转换，然后进行渲染
                    sws_scale(ms->img_convert_ctx,
                                (const unsigned char* const*)ms->pFrame_video->data,
                              ms->pFrame_video->linesize,
                              0,
                              ms->decode_v_ctx->height,
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


int MediaFile::InitMediaFile(void *_ms)
{

    MediaState *ms = (MediaState *)_ms;
    UVideoOutput* UVO = (UVideoOutput*)(ms->uVideoOutput);

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

    if(ret < 0){
        av_log(NULL, AV_LOG_FATAL, "DecoderInit error.\n");
        return -1;
    }

    //get media info
    MediaInfo mi;
    md.DecoderGetMediaInfo(&mi, ms->decode_v_ctx, ic->streams[ms->video_stream_index]);
    md.DecoderGetMediaInfo(&mi, ms->decode_a_ctx, ic->streams[ms->audio_stream_index]);

    ms->totalDuration = mi.duration;
    ms->totalDurationStr = std::move(mi.durationFormatStr);
    UVO->setTotalDuration(ms->totalDuration);
    UVO->setTotalDurationStr(ms->totalDurationStr.c_str());

    md.SWSInit2((void**)&ms->img_convert_ctx,
               &ms->out_buffer_video,
               &ms->pFrameRGB_video,
               ms->decode_v_ctx,
               ic->streams[ms->video_stream_index]);

    md.SWSInit2((void**)&ms->audio_convert_ctx,
               &ms->out_buffer_audio,
               &ms->pFrame_audio,
               ms->decode_a_ctx,
               ic->streams[ms->audio_stream_index]);

    //获取文件总时长
    if(ms->video_stream_index >= 0 )
    {
        ms->pFrame_video = av_frame_alloc();
        ms->packet = av_packet_alloc();
    }

    if(ms->audio_stream_index >= 0)
    {

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

    //create read thread
    ms->read_thr = std::thread(media_play_read_thread,ms);
    ms->read_thr.detach(); //分离线程，不阻塞
    return 1;
}
