#ifndef MEDIACAMERA_H
#define MEDIACAMERA_H

#include "ffmpeg_common.h"
#include <string>

class MediaCamera
{
public:
    MediaCamera();

    int InitCamera(void *_ms);

};

#endif // MEDIACAMERA_H
