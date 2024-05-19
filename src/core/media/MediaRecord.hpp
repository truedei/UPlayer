#ifndef MEDIARECORD_H
#define MEDIARECORD_H


#include "string"
#include "ffmpeg_common.h"

enum AV_RECORD_STATUS{
    AV_RECORD_STATUS_DESTROY = 0,//no
    AV_RECORD_STATUS_RUNING,      // 正在录像状态
    AV_RECORD_STATUS_PAUSE,       // 暂停录像状态
};

enum AV_AudioType {
    AV_VIDEO_TYPE_H264 = 0,
    AV_VIDEO_TYPE_H265,

    AV_AUDIO_TYPE_AAC,
    AV_AUDIO_TYPE_MP3,
    AV_AUDIO_TYPE_G711A,
};


typedef struct VideoEncObj {
    int codec_id;       // 0:H264,1:H265
    int profile_id;     // 0:BP,1:MP,2:HP
    int ratecontrol_id; // 0:CBR,1:VBR
    int bitrate;
    int width;
    int height;
    int framerate;
    int gop;
    int MaxQP;
    int MinQP;
    int keyinterval;
    int keyMinQP;
    int QPlevel;
} VideoEncObj;

typedef struct AudioEncObj {
    int _switch;     // 0:close,1:open
    int codec;       // 0:AAC,1:MP3,2:G.711A
    int bitrate;     //@audio_setup.json --> bitrate_list[]
    int sample_rate; //@audio_setup.json --> sample_rate_list[]
    int bit_width;
    int channel;
    int volume;
} AudioEncObj;



enum AV_MediaType {
    AV_MEDIA_TYPE_VIDEO = 0,
    AV_MEDIA_TYPE_AUDIO,

    AV_MEDIA_TYPE_AUDIO_AAC,
    AV_MEDIA_TYPE_AUDIO_MP3,
    AV_MEDIA_TYPE_AUDIO_G711A,

    AV_MEDIA_TYPE_VIDEO_H264_H,
    AV_MEDIA_TYPE_VIDEO_H264_I,
    AV_MEDIA_TYPE_VIDEO_H264_P,
    AV_MEDIA_TYPE_VIDEO_H265_H,
    AV_MEDIA_TYPE_VIDEO_H265_I,
    AV_MEDIA_TYPE_VIDEO_H265_P,
};

typedef struct AVData {
    AV_MediaType media_type;
    int frame_type; // only Video
    int64_t pts;
    int64_t dts;
    uint8_t *extra_data;
    int extra_size;
    uint8_t *data;
    int size;
    uint8_t *header; // sps pps
    float fps;
    int sample_rate;
    int duration;
} AVData;



class AVRecord
{

private:

    AVStream *pVideoStream;
    AVStream *pAudioStream;

    const AVOutputFormat * avof;

    int videoIndex;
    int audioIndex;

    uint64_t vpts;
    uint64_t apts;

    std::string recordFile;

    AV_RECORD_STATUS avRecordStatus;

    VideoEncObj *vencObj;
    AudioEncObj *aencObj;

    //开始推流的时间
    int64_t start_time_ = 0;


    //front avpacket pts
    int64_t front_pts = 0;

    AVStream *AddAVStream(AVFormatContext *octx, AVMediaType codec_type, int EncType);




public:

    int frameIndex = 0;
    AVFormatContext *octx;


    //encodec
    const AVCodec *codec = nullptr;
    AVCodecContext *enc_ctx = nullptr;

    AVFrame *frame = nullptr;
    AVPacket *pkt = nullptr;

    AVRecord();
    ~AVRecord();

    std::string getRecordFile() const;
    void setRecordFile(const std::string &newRecordFile);
    AV_RECORD_STATUS getAvRecordStatus() const;
    void setAvRecordStatus(AV_RECORD_STATUS newAvRecordStatus);



    VideoEncObj *getVencObj() const;
    void setVencObj(VideoEncObj *newVencObj);
    AudioEncObj *getAencObj() const;
    void setAencObj(AudioEncObj *newAencObj);


    int InitAVRecorder();
    int CloseAVRecorder();

    int WriteData(AVData *avData);
    int WriteData(AVFrame *frame);
};

#endif // MEDIARECORD_H
