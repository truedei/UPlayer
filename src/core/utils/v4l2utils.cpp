#include "v4l2utils.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

//for dir
#include <sys/types.h>
#include <dirent.h>


#include <string.h>

#include "QDebug"




vector<string> GetVideos()
{
    vector<string> vs;

    string dev_p = "/dev/";

    //1.遍历/dev/目录下的所有文件找到/dev/video开头的文件
    DIR *devDir = opendir(dev_p.c_str());
    if(devDir == NULL)
    {
        cout << "opendir " << dev_p << "error" << endl;
        return vs;
    }

    dirent *ent;
    if (devDir != NULL) {
        while ((ent = readdir(devDir)) != NULL) {

            //找到video开头的
            string dname = ent->d_name;
            if(dname.substr(0,5) == "video")
            {
//                cout << "dname=" <<  dev_p + dname << endl;
                vs.push_back(dev_p + dname);
            }
        }
    }

    closedir(devDir);
    return vs;
}


Camera GetCamInfo(string camPath)
{
    Camera cam;

    int camFd = open((char*)camPath.c_str(), O_RDWR);
    if(camFd < 0)
    {
        std::cout << "open " << camPath << "fail" << endl;
        return cam;
    }

    struct v4l2_capability cap;
    if (ioctl(camFd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        std::cout << "ERR VIDIOC_QUERYCAP failed " << endl;
        return cam;
    }


    // 判断是否拥有video能力
    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        std::cout << "not capabilities" << endl;
        return cam;
    }


    // 枚举该video设备的input
    struct v4l2_input input;

    input.index = 0;
    while(!ioctl(camFd, VIDIOC_ENUMINPUT, &input))
    {
        if(input.index == 0)
        {
            cam.devName = (char*)input.name;
        }

        ++input.index;
    }

    if(cam.devName.empty())
    {
        return cam;
    }

//    cout << "cap.card=" << cap.card << endl;

    cam.camName = (char*)(cap.card);
    if(cam.camName.find(":"))
    {
//        cout << "cam.camName.find(:)=" << cam.camName.find(":") << endl;
        cam.camName.erase(0,cam.camName.find(":")+1);

        // del start and end " "
        cam.camName.erase(0,cam.camName.find_first_not_of(" "));
        cam.camName.erase(cam.camName.find_last_not_of(" ") + 1);
    }
    cam.camPath = camPath;



    // 2. 获取摄像头支持的格式
    // #define VIDIOC_ENUM_FMT         _IOWR('V',  2, struct v4l2_fmtdesc)
    struct v4l2_fmtdesc fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.index = 0;
    while ( !ioctl(camFd, VIDIOC_ENUM_FMT, &fmt)) {

        VideoFmt vfmt;
        vfmt.index = fmt.index;
        vfmt.description = (char*)(fmt.description);

        char pix[10];
        unsigned char *pp = (unsigned char *) &fmt.pixelformat;
        sprintf(pix,"%c%c%c%c", pp[0], pp[1], pp[2], pp[3]);

        vfmt.pixelformat = (char*)(pix);

        if(vfmt.description == "YUYV 4:2:2")
        {
            vfmt.v_fmt_type = VIDEO_FMT_YUYV422;
        }else if(vfmt.description == "Motion-JPEG")
        {
            vfmt.v_fmt_type = VIDEO_FMT_MJPG;
        }else if(vfmt.description == "H.264")
        {
            vfmt.v_fmt_type = VIDEO_FMT_H264;
        }else if(vfmt.description == "HEVC")
        {
            vfmt.v_fmt_type = VIDEO_FMT_HEVC;
        }



//        cout << "获取摄像头支持的分辨率"  << endl;
        //获取摄像头支持的分辨率
        struct v4l2_frmsizeenum framsize;
        framsize.index = 0;
        framsize.pixel_format = fmt.pixelformat;
        while (!ioctl(camFd, VIDIOC_ENUM_FRAMESIZES, &framsize))
        {
//            cout << "framsize.index="  << framsize.index
//                 << ",framsize.type="  << framsize.type
//                 << ",framsize.pixel_format="  << vfmt.pixelformat
//                 << ",framsize.discrete.height="  << framsize.discrete.height
//                 << ",framsize.discrete.width="  << framsize.discrete.width
//                 << endl;
            VideoFrmsize vfsize;
            vfsize.index = framsize.index;
            vfsize.height = framsize.discrete.height;
            vfsize.width = framsize.discrete.width;

            struct v4l2_frmivalenum frmival;
            frmival.pixel_format = fmt.pixelformat;
            frmival.width = framsize.discrete.width;
            frmival.height = framsize.discrete.height;
            frmival.index = 0;

            while (!ioctl(camFd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival)){ //帧率
                if (frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE)
                {
                   int frameRate = frmival.discrete.denominator / frmival.discrete.numerator;
//                   cout << "index="<< frmival.index << ",frameRate:" << frameRate
//                        << ","  << framsize.discrete.height
//                        << "x"  << framsize.discrete.width
//                        << endl;
                   vfsize.frameRates.push_back(frameRate);
                }
                frmival.index++;
            }

            vfmt.frmsizes.push_back(vfsize);
            framsize.index++;
        }

        cam.vfmts.push_back(vfmt);
        fmt.index++;
     }




    close(camFd);
    return cam;
}

vector<Camera> GetAllVideoDevices()
{
    vector<Camera> cms;
    //1.get /dev/videoxx
    vector<string> videos = GetVideos();
    for(int i = 0; i < videos.size(); i++)
    {
//       std::cout << "videos=" << videos[i]<< endl;

       //找到真是的视频设备,存在/dev/videox不一定是真实的,所以需要打开检测一下

       Camera cam = GetCamInfo(videos[i]);
       if(cam.camName != "")
       {
           cms.push_back(cam);
           std::cout<< "cam.camPath =" <<   cam.camPath << ", cam.camName=" << cam.camName << ",cam.devName=" << cam.devName << endl;
       }
    }
    return cms;
}

json GetAllVideoDevicesToJson()
{
    json data;
    data["aa"]="basd";

    vector<Camera> cms = GetAllVideoDevices();

    for(int i = 0; i < cms.size(); i++)
    {
        cout <<"============================================================" <<endl;
        std::cout << "cam=" << cms[i].camName  << ",camPath=" << cms[i].camPath << ",cms[i].vfmts.size()="<<cms[i].vfmts.size()<< '\n';
        for(int j = 0; j < cms[i].vfmts.size(); j++)
        {
            cout <<"============================================================" <<endl;
            std::cout << "fmt index=" << cms[i].vfmts[j].index  <<
                         ",description=" << cms[i].vfmts[j].description <<
                         ",pixelformat=" << cms[i].vfmts[j].pixelformat <<
                         ",v_fmt_type=" << cms[i].vfmts[j].v_fmt_type <<
                         '\n';

            for(int k = 0; k < cms[i].vfmts[j].frmsizes.size(); k++)
            {
                cout << "framsize.discrete.height="  << cms[i].vfmts[j].frmsizes[k].height
                     << ",framsize.discrete.width="  << cms[i].vfmts[j].frmsizes[k].width
                     << endl;
            }
        }

    }

    return data;
}

#include <sys/mman.h>



//Collect Video Data

int CollectVideoData(char *devName)
{

    GetAllVideoDevicesToJson();

    char filename[128];


    //open device
    int fd = open(devName, O_RDWR);
    if(fd < 0)
    {
        qDebug() << "open " << devName << " error" ;
        return -1;
    }

    //设置采集格式
    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 摄像头采集


    //设置视频采集格式
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422P;
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_HEVC;
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;

//OK
    //ffplay -f rawvideo -pixel_format yuyv422 -video_size 1280x720 -i out.yuv
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    //设置采集的像素宽高
    vfmt.fmt.pix.width = 1280;
    vfmt.fmt.pix.height = 720;
    sprintf(filename,"/data/project/Uplayer/UplayerCodes/videotest/out.yuv");

//OK
//    //ffplay -f rawvideo -pixel_format yuyv422 -video_size 1280x720 -i out.h264
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264_NO_SC;
//    //设置采集的像素宽高
//    vfmt.fmt.pix.width = 1920;
//    vfmt.fmt.pix.height = 1080;
//    sprintf(filename,"/data/project/Uplayer/UplayerCodes/videotest/out.h264");

//NO
//    //ffplay -f rawvideo -pixel_format yuyv422 -video_size 1280x720 -i out.h264
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
//    //设置采集的像素宽高
//    vfmt.fmt.pix.width = 320;
//    vfmt.fmt.pix.height = 240;
//    sprintf(filename,"/data/project/Uplayer/UplayerCodes/videotest/out.jpg");

//NO
//    //ffplay -f rawvideo -pixel_format yuyv422 -video_size 1280x720 -i out.h264
//    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_HEVC;
//    //设置采集的像素宽高
//    vfmt.fmt.pix.width = 320;
//    vfmt.fmt.pix.height = 240;
//    sprintf(filename,"/data/project/Uplayer/UplayerCodes/videotest/out.h265");


    int ret = ioctl(fd, VIDIOC_S_FMT, &vfmt);
    if(ret < 0)
    {
        qDebug() << "设置视频采集格式  " << " error" ;
        return -1;
    }

    //申请内核空间
    int buff_count = 4;

    struct v4l2_requestbuffers vbuff;
    vbuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuff.count = buff_count;//申请4个缓冲区
    vbuff.memory = V4L2_MEMORY_MMAP; //映射方式
    ret = ioctl(fd, VIDIOC_REQBUFS, &vbuff);
    if(ret < 0)
    {
        qDebug() << "申请内核空间 error" ;
        return -1;
    }

    //映射 VIDIOC_QUERYBUF VIDIOC_QBUF

    //后期采集数据,就是从这4个缓冲区采集数据
    unsigned char *mptr[buff_count];//保存映射后的用户空间的首地址
    unsigned int  size[buff_count];

    struct v4l2_buffer mapbuff;
    // init type,index
    mapbuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for (int i = 0; i < buff_count; ++i) {
        mapbuff.index = i;

        //从内核空间中查询一个空间做映射
        ret = ioctl(fd, VIDIOC_QUERYBUF, &mapbuff);
        if(ret < 0)
        {
            qDebug() << "查询内核空间 error" ;
            return -1;
        }
        mptr[i] = (unsigned char *)mmap(NULL,
                                        mapbuff.length,
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED,
                                        fd,
                                        mapbuff.m.offset);
        size[i] = mapbuff.length;

        ret = ioctl(fd, VIDIOC_QBUF, &mapbuff);
        if(ret < 0)
        {
            qDebug() << "放回mapbuff失败" ;
            return -1;
        }
    }


    //开始采集
    // VIDIOC_QBUF VIDIOC_DQBUF VIDIOC_STREAMON VIDIOC_STREAMOFF
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if(ret < 0)
    {
        qDebug() << "VIDIOC_STREAMON 失败" ;
        return -1;
    }

    int read_num = 0;
    FILE *file = fopen(filename, "w+");
    while (read_num < 10) {
        // 从队列提取一帧数据
        struct v4l2_buffer readbuffer;
        readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(fd, VIDIOC_DQBUF, &readbuffer);
        if(ret < 0)
        {
            qDebug() << "从队列提取一帧数据 失败" ;
            return -1;
        }

//        printf("hex:");
//        for (int var = 0; var < 100; ++var) {
//            printf("0x%x",mptr[readbuffer.index][var]);
//        }
//        printf("\n");
        qDebug() << "readbuffer.index=" << readbuffer.index << ",readbuffer.length=" << readbuffer.length ;


        fwrite(mptr[readbuffer.index], readbuffer.length, 1, file);

        //同志内核已经使用完毕
        ret = ioctl(fd, VIDIOC_QBUF, &readbuffer);
        if(ret < 0)
        {
            qDebug() << "VIDIOC_QBUF 失败" ;
            return -1;
        }
        read_num++;
    }

    fclose(file);


    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if(ret < 0)
    {
        qDebug() << "VIDIOC_STREAMOFF 失败" ;
        return -1;
    }

    for (int i = 0; i < buff_count; ++i) {
        munmap(mptr[i], size[i]);
    }

    if(fd)
    {
        close(fd);
    }

    qDebug() << "OKOKOKOKOKOK" ;
    return 1;
}
