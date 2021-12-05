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
        main.cpp

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
    main.h


DESTDIR = $$PWD/../lib/

# Include path
INCLUDEPATH += $$PWD/../include/ffmpeg
DEPENDPATH += $$PWD/../include/ffmpeg

LIBS += -L $$PWD/../lib/
LIBS += -lavcodec \
            -lavdevice \
            -lavformat \
            -lavfilter \
            -lswscale \
            -lswresample \
            -lavutil \
            -lpthread

message("=================")
message($$LIBS)
message("=================")


#图标
RC_FILE = logo.rc
