QT += quick multimedia widgets
CONFIG += c++11
CONFIG += console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    core/main.cpp \
    core/media/MediaCamera.cpp \
    core/media/MediaCore.cpp \
    core/media/MediaFFmpeg.cpp \
    core/media/MediaFile.cpp \
    core/media/MediaRecord.cpp \
    core/qt/AVPlayerCore.cpp \
    core/qt/AVPlayerUtils.cpp \
    core/qt/AVRecordConnect.cpp \
    core/qt/AVRecordPainted.cpp \
    core/qt/AVRecordPlay.cpp \
    core/qt/App.cpp \
    core/utils/configutils.cpp \
    core/utils/v4l2utils.cpp

HEADERS += \
    core/include/common.h \
    core/include/ffmpeg_common.h \
    core/include/nlohmann/json.hpp \
    core/include/nlohmann/json_fwd.hpp \
    core/media/LiveStreamer.hpp \
    core/media/MediaCamera.hpp \
    core/media/MediaCore.hpp \
    core/media/MediaFFmpeg.hpp \
    core/media/MediaFile.hpp \
    core/media/MediaRecord.hpp \
    core/qt/AVPlayerCore.hpp \
    core/qt/AVPlayerUtils.hpp \
    core/qt/AVRecordConnect.hpp \
    core/qt/AVRecordPainted.hpp \
    core/qt/AVRecordPlay.hpp \
    core/qt/App.h \
    core/utils/configutils.h \
    core/utils/v4l2utils.h


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../README.md \
    AppIcon.rc \
    Image/u_player_logo.png \
    Image/awb4q-fj9ks-002.ico \
    Image/u_player_logo.ico \
    images/awb4q-fj9ks-002.ico \
    images/dir.png \
    images/icon_Menu.png \
    images/image.jpg \
    images/play_fast_forward.png \
    images/play_fast_reverse.png \
    images/play_pause.png \
    images/play_start.png \
    images/play_stop.png \
    images/play_volume_close.png \
    images/play_volume_high.png \
    images/play_volume_low.png \
    images/record_1.png \
    images/record_2.png \
    images/snapshot_1.png \
    images/snapshot_2.png \
    images/theme.png \
    images/u_player_logo.ico \
    images/u_player_logo.png \
    images/window_close.png \
    images/window_max.png \
    images/window_min.png \
    logo.rc \
    main.qmlc \
    qml/MainPage.qml \
    qml/director/DirectorMain.qml \
    qml/mediaplay/Button.qml \
    qml/mediaplay/CMsgBox.qml \
    qml/mediaplay/FileOpen.qml \
    qml/mediaplay/MediaPlayMain.qml \
    qml/mediaplay/menuBar.qml \
    qml/record/RecordMain.qml \
    qml/record/RecordSettings.qml \
    qml/test.qml



# Include path

#INCLUDEPATH += /data/soft/build_ffmpeg/FFmpeg-n5.1.2/_install/include
#DEPENDPATH += /data/soft/build_ffmpeg/FFmpeg-n5.1.2/_install/include

#INCLUDEPATH += /data/soft/build_ffmpeg/SDL-release-2.24.1/_install/include/SDL2
#DEPENDPATH += /data/soft/build_ffmpeg/SDL-release-2.24.1/_install/include/SDL2

#INCLUDEPATH += /data/soft/build_ffmpeg/fdk-aac-2.0.2/_install/include
#DEPENDPATH += /data/soft/build_ffmpeg/fdk-aac-2.0.2/_install/include

#INCLUDEPATH += /data/soft/build_ffmpeg/x264/_install/include
#DEPENDPATH += /data/soft/build_ffmpeg/x264/_install/include

#INCLUDEPATH += /data/soft/build_ffmpeg/x265_3.5/_install/include
#DEPENDPATH += /data/soft/build_ffmpeg/x265_3.5/_install/include


#DESTDIR = /data/soft/build_ffmpeg/FFmpeg-n5.1.2/_install/lib
#DESTDIR += /data/soft/build_ffmpeg/SDL-release-2.24.1/_install/lib
#DESTDIR += /data/soft/build_ffmpeg/fdk-aac-2.0.2/_install/lib
#DESTDIR += /data/soft/build_ffmpeg/x264/_install/lib
#DESTDIR += /data/soft/build_ffmpeg/x265_3.5/_install/lib

#LIBS += -L/data/soft/build_ffmpeg/FFmpeg-n5.1.2/_install/lib
#LIBS += -lavcodec
#LIBS += -lavdevice
#LIBS += -lavfilter
#LIBS += -lavformat
#LIBS += -lavutil
#LIBS += -lpostproc
#LIBS += -lswresample
#LIBS += -lswscale
#LIBS += -lx265
#LIBS += -lSDL2

BUILD_SOURCE_PATH = $$PWD/../build_source/

INCLUDEPATH += $${BUILD_SOURCE_PATH}/FFmpeg-n5.1.2/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/FFmpeg-n5.1.2/include

INCLUDEPATH += $${BUILD_SOURCE_PATH}/SDL-release-2.24.1/include/SDL2
DEPENDPATH += $${BUILD_SOURCE_PATH}/SDL-release-2.24.1/include/SDL2

INCLUDEPATH += $${BUILD_SOURCE_PATH}/fdk-aac-2.0.2/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/fdk-aac-2.0.2/include

INCLUDEPATH += $${BUILD_SOURCE_PATH}/x264/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/x264/include

INCLUDEPATH += $${BUILD_SOURCE_PATH}/x265_3.5/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/x265_3.5/include

#openssl
INCLUDEPATH += $${BUILD_SOURCE_PATH}/openssl-1.0.2s/_install/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/openssl-1.0.2s/_install/include

#zlib
INCLUDEPATH += $${BUILD_SOURCE_PATH}/zlib-1.2.13/_install/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/zlib-1.2.13/_install/include

#librtmp
INCLUDEPATH += $${BUILD_SOURCE_PATH}/librtmp/rtmpdump/librtmp/_install/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/librtmp/rtmpdump/librtmp/_install/include

INCLUDEPATH += ./core/include
DEPENDPATH += ./core/include

INCLUDEPATH += ./core/
DEPENDPATH += ./core/

DESTDIR =

DESTDIR +=  $${BUILD_SOURCE_PATH}/FFmpeg-n5.1.2/lib
DESTDIR += $${BUILD_SOURCE_PATH}/SDL-release-2.24.1/lib
DESTDIR += $${BUILD_SOURCE_PATH}/fdk-aac-2.0.2/lib
DESTDIR += $${BUILD_SOURCE_PATH}/x264/lib
DESTDIR += $${BUILD_SOURCE_PATH}/x265_3.5/lib

DESTDIR += $${BUILD_SOURCE_PATH}/openssl-1.0.2s/_install/lib
DESTDIR += $${BUILD_SOURCE_PATH}/zlib-1.2.13/_install/lib
DESTDIR += $${BUILD_SOURCE_PATH}/librtmp/rtmpdump/librtmp/lib


LIBS += -L$${BUILD_SOURCE_PATH}/FFmpeg-n5.1.2/lib
LIBS += -lavcodec
LIBS += -lavdevice
LIBS += -lavfilter
LIBS += -lavformat
LIBS += -lavutil
LIBS += -lpostproc
LIBS += -lswresample
LIBS += -lswscale
LIBS += -lx265
LIBS += -lSDL2
LIBS += -lasound

LIBS += -lz
LIBS += -lrtmp
#LIBS += -lssl
#LIBS += -lcrypto


message("=================")
message($$LIBS)
message("=================")
 

#图标
RC_FILE = logo.rc
