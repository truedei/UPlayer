#ifndef V4L2UTILS_H
#define V4L2UTILS_H

#include <vector>
#include "string"
#include "iostream"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;


enum VIDEO_FMT_TYPE{
    VIDEO_FMT_YUYV422,
    VIDEO_FMT_MJPG,
    VIDEO_FMT_H264,
    VIDEO_FMT_HEVC,
};


struct VideoFrmsize
{
    int index;
    int height;
    int width;
    vector<int> frameRates;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoFrmsize,index, height, width, frameRates)
};

struct VideoFmt
{
    int index;
    string description;
    string pixelformat;
    VIDEO_FMT_TYPE v_fmt_type;
    vector<VideoFrmsize> frmsizes;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(VideoFmt,index, description, pixelformat, v_fmt_type,frmsizes)
};

struct Camera
{
    std::string camName;//camera name
    string devName;//camera dev name
    string camPath;//cam dev path
    vector<VideoFmt> vfmts;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Camera,camName, devName, camPath, vfmts)
};

vector<Camera> GetAllVideoDevices();

json GetAllVideoDevicesToJson();



int CollectVideoData(char *devName);

#endif // V4L2UTILS_H
