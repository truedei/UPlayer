#include "MediaCore.hpp"
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include "iostream"
#include "unistd.h"
#include "map"
#include "QImage"

#include "qt/AVPlayerCore.hpp"
#include "qt/AVRecordPainted.hpp"
#include "MediaCamera.hpp"
#include "MediaFile.hpp"


static MediaCamera mc;
static MediaFile mf;

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
    std::cout << "MEDIA_stream_exit success !" << std::endl;
}


MediaState *MEDIA_stream_open(MEDIA_TYPE media_t,std::string filename,void *uVideoOutput)
{
    MediaState *ms;
    ms = (MediaState*)av_mallocz(sizeof (MediaState));
    if(!ms)
    {
        return NULL;
    }

    ms->media_type = media_t;
    ms->model_type = MODEL_TYPE_NULL;

    ms->video_stream_index = -1;
    ms->audio_stream_index = -1;
    ms->filename = filename;

    if (ms->filename.empty())
    {
        return NULL;
    }


    if(ms->media_type == MEDIA_TYPE_MEDIA_PLAY)
    {
        if(ms->uVideoOutput == NULL)
        {
            ms->uVideoOutput = (UVideoOutput*)(uVideoOutput);
        }
        mf.InitMediaFile((void*)ms);
    }
    else if(ms->media_type == MEDIA_TYPE_RECORD_PLAY)
    {
        if(ms->uVideoOutput == NULL)
        {
            ms->uVideoOutput = (AVRecordPainted*)(uVideoOutput);
        }

        mc.InitCamera((void*)ms);
    }
    return ms;
}

