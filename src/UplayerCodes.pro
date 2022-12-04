QT += quick multimedia widgets
CONFIG += c++11

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
        UPlayer.cpp \
        UVideoOutput.cpp \
        main.cpp \
        media.cpp


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
    Image/u_player_logo.png \
    Image/awb4q-fj9ks-002.ico \
    Image/u_player_logo.ico

HEADERS += \
    UPlayer.h \
    UVideoOutput.h \
    common.h \
    media.h

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

INCLUDEPATH += /$${BUILD_SOURCE_PATH}/x264/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/x264/include

INCLUDEPATH += $${BUILD_SOURCE_PATH}/x265_3.5/include
DEPENDPATH += $${BUILD_SOURCE_PATH}/x265_3.5/include

DESTDIR =

DESTDIR +=  $${BUILD_SOURCE_PATH}/FFmpeg-n5.1.2/lib
DESTDIR += $${BUILD_SOURCE_PATH}/SDL-release-2.24.1/lib
DESTDIR += $${BUILD_SOURCE_PATH}/fdk-aac-2.0.2/lib
DESTDIR += $${BUILD_SOURCE_PATH}/x264/lib
DESTDIR += $${BUILD_SOURCE_PATH}/x265_3.5/lib

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



message("=================")
message($$LIBS)
message("=================")
 

#图标
RC_FILE = logo.rc
