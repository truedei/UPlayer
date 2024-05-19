#include "MediaRecord.hpp"
#include "common.h"
#include <unistd.h>

#include "qdebug.h"


AVRecord::AVRecord()
{

    // init param
    octx = NULL;
    pVideoStream = NULL;
    pAudioStream = NULL;
    avof=NULL;
    videoIndex = -1;
    audioIndex = -1;

    //开始推流的时间
    start_time_ = 0;

    recordFile = "";
    avRecordStatus = AV_RECORD_STATUS_DESTROY;
}

AVRecord::~AVRecord()
{
    // init param
    octx = NULL;
    pVideoStream = NULL;
    pAudioStream = NULL;
    avof=NULL;
    videoIndex = -1;
    audioIndex = -1;

    //开始推流的时间
    start_time_ = 0;


    recordFile = "";
    avRecordStatus = AV_RECORD_STATUS_DESTROY;
}



std::string AVRecord::getRecordFile() const
{
    return recordFile;
}

void AVRecord::setRecordFile(const std::string &newRecordFile)
{
    recordFile = newRecordFile;
}

AV_RECORD_STATUS AVRecord::getAvRecordStatus() const
{
    return avRecordStatus;
}

void AVRecord::setAvRecordStatus(AV_RECORD_STATUS newAvRecordStatus)
{
    avRecordStatus = newAvRecordStatus;
}

int AVRecord::WriteData(AVData *avData)
{
    if(this->getAvRecordStatus() != AV_RECORD_STATUS_RUNING)
    {
        return -1;
    }

    int ret = 0;
    AVPacket pkt;
    av_init_packet(&pkt);
    AVRational avrVideo = {1, 90000}; // 45000-60fps, 90000-30fps
    AVRational avrAudio = {1, 48000}; // 45000-60fps, 90000-30fps

    uint64_t duration = 0;
    if (avData->media_type == AV_MEDIA_TYPE_VIDEO)
    {
        pkt.stream_index = this->videoIndex;
        pkt.flags = 0;

        if (avData->fps != 0)
        {
            duration = 1000000 / (avData->fps);
            this->vpts += duration;
        }

        pkt.pts = av_rescale_q(this->vpts, AV_TIME_BASE_Q, avrVideo);
        pkt.duration = av_rescale_q(duration, AV_TIME_BASE_Q, avrVideo);

        // DB_PRT("[video] %s ---> pts=%llu,duration=%llu,this->vidpts=%llu.\n", this->GetRecordFile().c_str(), pkt.pts , pkt.duration, this->vidpts);
    }
    else if (AV_MEDIA_TYPE_AUDIO == avData->media_type)
    {
        pkt.stream_index = this->audioIndex;
        pkt.flags = 1;
        if (avData->sample_rate != 0)
        {
            duration = (1000000 * 1024) / (avData->sample_rate);
            this->apts += duration;
        }

        pkt.pts = av_rescale_q(this->vpts, AV_TIME_BASE_Q, avrAudio);
        pkt.duration = av_rescale_q(duration, AV_TIME_BASE_Q, avrAudio);

        // DB_PRT("[audio] %s ---> pts=%llu,duration=%llu,this->audpts=%llu.\n", this->GetRecordFile().c_str(), pkt.pts , pkt.duration,this->audpts);
    }
    pkt.size = avData->size;
    pkt.data = avData->data;

    pkt.dts = pkt.pts;
    pkt.pos = -1;

    ret = av_interleaved_write_frame(this->octx, &pkt);
    if (ret < 0)
    {
         char buf[1024];
         //获取错误信息
         av_strerror(ret, buf, sizeof(buf));

         ERR_PRT("av_interleaved_write_frame error errNum=%d. err msg=%s\n", ret, buf);
//         if(ret == -5)
//         {
//             this->Close_AVRecorder();
//         }
        return ret;
    }

    // 推送数据流 会自动释放空间 不需要调用
    av_packet_unref(&pkt);



    return 1;
}


int AVRecord::WriteData(AVFrame *pFrameYUV)
{
    if(this->getAvRecordStatus() != AV_RECORD_STATUS_RUNING)
    {
        return -1;
    }
    int ret = 0;

    ret = avcodec_send_frame(this->enc_ctx, pFrameYUV);
    if (ret < 0)
    {
        printf("failed to encode.\n");
        return -1;
    }

//    AVRational avrVideo = {1, 90000}; // 45000-60fps, 90000-30fps

    AVRational avrVideo = {1, 100000}; // 45000-60fps, 90000-30fps
    uint64_t duration = 0;

#if 1
    if (avcodec_receive_packet(this->enc_ctx, pkt) >= 0)
    {
        pkt->stream_index = this->videoIndex;
        pkt->flags = 0;

        duration = 1000000 / (10); //时间间隔
        this->vpts += duration;

        // 设置输出DTS,PTS
        pkt->dts = pkt->pts = av_rescale_q(this->vpts, AV_TIME_BASE_Q, avrVideo);
        pkt->duration = av_rescale_q(duration, AV_TIME_BASE_Q, avrVideo);

        ret = av_interleaved_write_frame(this->octx, pkt);
        if (ret < 0)
        {
            printf("send packet failed: %d\n", ret);
        }
        else
        {
//            printf("send %5d packet successfully!\n", frameIndex);
        }

//        AVData avdata;
//        avdata.data = pkt->data;
//        avdata.size = pkt->size;
//        avdata.pts = 25;
//        avdata.media_type = AV_MEDIA_TYPE_VIDEO;

//        WriteData(&avdata);
    }

#endif

#if 0
    int ii = 0;
    /* read all the available output packets (in general there may be any number of them */
    // 编码和解码都是一样的，都是send 1次，然后receive多次, 直到AVERROR(EAGAIN)或者AVERROR_EOF
    while (ret >= 0) {
        ret = avcodec_receive_packet(this->enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            fprintf(stderr, "Error encoding audio frame\n");
            return -1;
        }

        qDebug() << "ii=" << ii ;
        ii++;
    }

#endif

    return 1;
}


AVStream * AVRecord::AddAVStream(AVFormatContext *octx, AVMediaType codec_type, int EncType)
{
    // init Stream Info
    AVStream *outAVStream = avformat_new_stream(octx, NULL);
    outAVStream->codecpar->codec_tag = 0;

    if (AVMEDIA_TYPE_VIDEO == codec_type)
    {
        outAVStream->codecpar->codec_type = (AVMediaType)AVMEDIA_TYPE_VIDEO;
        outAVStream->codecpar->codec_tag = 0;
        outAVStream->codecpar->format = (AVPixelFormat)AV_PIX_FMT_YUV420P;
        outAVStream->codecpar->bit_rate = vencObj->bitrate;
        outAVStream->codecpar->sample_aspect_ratio.num = 0;
        outAVStream->codecpar->sample_aspect_ratio.den = 0;
        outAVStream->codecpar->field_order = (AVFieldOrder)AV_FIELD_PROGRESSIVE;
        outAVStream->codecpar->video_delay = 0;
        outAVStream->codecpar->width = vencObj->width;
        outAVStream->codecpar->height = vencObj->height;

        outAVStream->time_base = AVRational{1,90000};

        if (EncType == AV_VIDEO_TYPE_H264)
        {
            outAVStream->codecpar->codec_id = (AVCodecID)AV_CODEC_ID_H264;
            outAVStream->codecpar->level = 51; // 5.1

            // ["BP","MP", "HP"]
            switch (vencObj->profile_id)
            {
            case 1: // BP
                outAVStream->codecpar->profile = FF_PROFILE_H264_BASELINE;
                break;
            case 2: // MP
                outAVStream->codecpar->profile = FF_PROFILE_H264_MAIN;
                break;
            case 3: // HP
                outAVStream->codecpar->profile = FF_PROFILE_H264_HIGH;
                break;
            default:
                outAVStream->codecpar->profile = FF_PROFILE_H264_BASELINE;
                break;
            }
        }
        else if (EncType == AV_VIDEO_TYPE_H265)
        {
            outAVStream->codecpar->codec_id = (AVCodecID)AV_CODEC_ID_HEVC;
            outAVStream->codecpar->level = 128;
            outAVStream->codecpar->profile = FF_PROFILE_HEVC_MAIN;
        }
    }

#if 0
    if (AVMEDIA_TYPE_AUDIO == codec_type)
    {
        outAVStream->codecpar->codec_type = (AVMediaType)AVMEDIA_TYPE_AUDIO;
        outAVStream->codecpar->codec_tag = 0;
        outAVStream->codecpar->format = (AVSampleFormat)AV_SAMPLE_FMT_S16P;
        outAVStream->codecpar->bit_rate = aencObj->bitrate;
        outAVStream->codecpar->sample_rate = aencObj->sample_rate;
        outAVStream->codecpar->channels = 2;
        outAVStream->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
        outAVStream->codecpar->frame_size = 1024;

        if (EncType == AV_AUDIO_TYPE_AAC)
        {
            outAVStream->codecpar->codec_id = (AVCodecID)AV_CODEC_ID_AAC;
            outAVStream->codecpar->profile = 1;
            outAVStream->codecpar->level = -99;
        }
        else if (EncType == AV_AUDIO_TYPE_MP3)
        {
            outAVStream->codecpar->codec_id = (AVCodecID)AV_CODEC_ID_MP3;
            // outAVStream->codecpar->format = (AVSampleFormat)AV_SAMPLE_FMT_FLTP;
            outAVStream->codecpar->profile = 0;
            outAVStream->codecpar->level = -99;
        }
        else if (EncType == AV_AUDIO_TYPE_G711A)
        {
            outAVStream->codecpar->codec_id = (AVCodecID)AV_CODEC_ID_PCM_ALAW; // G711A
            // outAudio->codecpar->codec_id = AV_CODEC_ID_PCM_MULAW;  //G711U
            outAVStream->codecpar->sample_rate = 8000; // default sample_rate:8000
        }
    }

#endif


    return outAVStream;
}


VideoEncObj *AVRecord::getVencObj() const
{
    return vencObj;
}

void AVRecord::setVencObj(VideoEncObj *newVencObj)
{
    vencObj = newVencObj;
}

AudioEncObj *AVRecord::getAencObj() const
{
    return aencObj;
}

void AVRecord::setAencObj(AudioEncObj *newAencObj)
{
    aencObj = newAencObj;
}

#include <libavutil/time.h>

int AVRecord::InitAVRecorder()
{
    DB_PRT("in InitAVRecorder \n");

    int ret = -1;

    this->avof = av_guess_format(nullptr, this->getRecordFile().c_str(), nullptr);
    if(!this->avof)
    {
        ERR_PRT("av_guess_format error \n");
        return -1;
    }
    DB_PRT("av_guess_format success! \n");

    // create output stream
    ret = avformat_alloc_output_context2(&(this->octx), avof, NULL, this->getRecordFile().c_str());
    if(!this->octx)
    {
        ERR_PRT("avformat_alloc_output_context2 error \n");
        return -1;
    }
    DB_PRT("avformat_alloc_output_context2 success! \n");

    // create a/v stream
    this->pVideoStream = AddAVStream(this->octx,AVMEDIA_TYPE_VIDEO, AV_VIDEO_TYPE_H264);
    if(this->pVideoStream == NULL)
    {
        ERR_PRT("AddAVStream pVideoStream error \n");
        return -1;
    }
    DB_PRT("AddAVStream pVideoStream success! \n");
    this->videoIndex = this->pVideoStream->index;

    //open avio
    ret = avio_open(&(this->octx->pb), this->getRecordFile().c_str(), AVIO_FLAG_WRITE);
    if(ret < 0)
    {
        ERR_PRT("avio_open error \n");
        return ret;
    }
    DB_PRT("avio_open success! \n");

    // write header
    AVDictionary *opt = NULL;
    ret = avformat_write_header(this->octx, &opt);
    if(ret < 0)
    {
        ERR_PRT("avformat_write_header error \n");
        return -1;
    }
    DB_PRT("avformat_write_header success! \n");

    av_dump_format(this->octx, 0, this->getRecordFile().c_str(), 1);


    this->apts = 0;
    this->vpts = 0;


    /*
     *
     *
     *
     **/

    //
    printf("\n");
#if 1
    DB_PRT("####################################### \n");
    DB_PRT("####################################### \n");
    DB_PRT("####################################### \n");

    /* 查找指定的编码器 */
//    codec = avcodec_find_encoder_by_name(codec_name);
//    AV_CODEC_ID_H264
    this->codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!this->codec) {
        fprintf(stderr, "Codec AV_CODEC_ID_H264 not found. \n");
        exit(1);
    }

    DB_PRT("avcodec_find_encoder AV_CODEC_ID_H264 codec success! \n");

    this->enc_ctx = avcodec_alloc_context3(this->codec);
    if (!this->enc_ctx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    DB_PRT("avcodec_alloc_context3 enc_ctx success! \n");

    /* 设置分辨率*/
    this->enc_ctx->width = 1280;
    this->enc_ctx->height = 720;

    /* 设置time base ，注意timebase的正确设置，会影响码率的输出, 即是AVFrame的pts的timebase需要和enc_ctx->time_base一致*/
    this->enc_ctx->time_base = (AVRational){1, 1000}; // 和AVFrame的pts相同，这样不需要做时间戳的转换
    this->enc_ctx->framerate = (AVRational){25, 1};

    /* 设置I帧间隔
     * 如果frame->pict_type设置为AV_PICTURE_TYPE_I, 则忽略gop_size的设置，一直当做I帧进行编码
     */
    this->enc_ctx->gop_size = 25;   // I帧间隔
    this->enc_ctx->max_b_frames = 0; // 如果不想包含B帧则设置为0
    this->enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    AVDictionary *dict = NULL;

    //
    if (codec->id == AV_CODEC_ID_H264) {
        // 相关的参数可以参考libx264.c的 AVOption options
        // ultrafast all encode time:2270ms
        // medium all encode time:5815ms
        // veryslow all encode time:19836ms

//        int av_dict_set(AVDictionary **pm, const char *key, const char *value, int flags);

//        ret = av_dict_set(&(enc_ctx->priv_data), "preset", "medium", 0);


        ret = av_dict_set(&dict, "preset", "medium", 0);
        if(ret != 0) {
            printf("av_opt_set preset failed\n");
        }
//        ret = av_opt_set(enc_ctx->priv_data, "profile", "main", 0); // 默认是high
        ret = av_dict_set(&dict, "profile", "main", 0);
        if(ret != 0) {
            printf("av_opt_set profile failed\n");
        }
//        ret = av_opt_set(enc_ctx->priv_data, "tune","zerolatency",0); // 直播是才使用该设置
//        ret = av_opt_set(enc_ctx->priv_data, "tune","film",0); //  画质film
        ret = av_dict_set(&dict, "tune", "zerolatency", 0);
        if(ret != 0) {
            printf("av_opt_set tune failed\n");
        }

    }

    /*
     * 设置编码器参数
    */
    /* 设置bitrate */
    this->enc_ctx->bit_rate = 3000000;
//    enc_ctx->rc_max_rate = 3000000;
//    enc_ctx->rc_min_rate = 3000000;
//    enc_ctx->rc_buffer_size = 2000000;
//    enc_ctx->thread_count = 4;  // 开了多线程后也会导致帧输出延迟, 需要缓存thread_count帧后再编程。
//    enc_ctx->thread_type = FF_THREAD_FRAME; // 并 设置为FF_THREAD_FRAME
    /* 对于H264 AV_CODEC_FLAG_GLOBAL_HEADER  设置则只包含I帧，此时sps pps需要从enc_ctx->extradata读取
     *  不设置则每个I帧都带 sps pps sei
     */
//    enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; // 存本地文件时不要去设置


    /* 将enc_ctx和codec进行绑定 */
    ret = avcodec_open2(this->enc_ctx, this->codec, &dict);
    if (ret < 0) {
//        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
       fprintf(stderr, "Could not open codec: %d\n", (ret));
        exit(1);
    }
    DB_PRT("avcodec_open2 enc_ctx codec success! \n");

    // 分配pkt和frame
    this->pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    this->frame = av_frame_alloc();
    if (!this->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    // 为frame分配buffer
    this->frame->format = this->enc_ctx->pix_fmt;
    this->frame->width  = this->enc_ctx->width;
    this->frame->height = this->enc_ctx->height;
    ret = av_frame_get_buffer(this->frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
#endif

    // 记录当前时间戳
    this->start_time_ = av_gettime();

    DB_PRT("this->start_time_=%lld \n", this->start_time_);


    this->setAvRecordStatus(AV_RECORD_STATUS_RUNING);
    return 1;
}

int AVRecord::CloseAVRecorder()
{
    this->setAvRecordStatus(AV_RECORD_STATUS_DESTROY);
    sleep(2);

    if (this->octx)
    {
        av_write_trailer(this->octx);
        if (this->octx && !(this->octx->oformat->flags & AVFMT_NOFILE))
        {
            avio_close(this->octx->pb);
        }
        avformat_free_context(this->octx);
    }

    this->videoIndex = 0;
    this->audioIndex = 0;
    this->octx = NULL;

    DB_PRT("CloseAVRecorder() [%s] success! \n", this->getRecordFile().c_str());

    return 1;
}





















