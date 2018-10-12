TEMPLATE = app

QT += qml quick

CONFIG += c++11

SOURCES += main.cpp \
    XVideo.cpp \
    XVideoThread.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    XVideo.h \
    XVideoThread.h


win32{
INCLUDEPATH += $$PWD/ffmpeg/include_
#INCLUDEPATH += $$PWD/SDL2/include
#INCLUDEPATH += $$PWD/portaudio/include
LIBS += $$PWD/ffmpeg/lib/avcodec.lib \
        $$PWD/ffmpeg/lib/avdevice.lib \
        $$PWD/ffmpeg/lib/avfilter.lib \
        $$PWD/ffmpeg/lib/avformat.lib \
        $$PWD/ffmpeg/lib/avutil.lib \
        $$PWD/ffmpeg/lib/postproc.lib \
        $$PWD/ffmpeg/lib/swresample.lib \
        $$PWD/ffmpeg/lib/swscale.lib \
#        $$PWD/SDL2/lib/x86/SDL2.lib     \
#        $$PWD/portaudio/lib/portaudio_x86.lib
}

unix{
INCLUDEPATH += /usr/local/ffmpeg/include
#INCLUDEPATH += /usr/include/SDL2/
LIBS += -L/usr/local/ffmpeg/lib/ \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale \
#        -L$$/usr/lib/i386-linux-gnu/ -lSDL2
}

android{
INCLUDEPATH += $$PWD/ffmpeg/include
LIBS +=  $$PWD/ffmpeg/lib/libavcodec-57.so \
        $$PWD/ffmpeg/lib/libavdevice-57.so \
        $$PWD/ffmpeg/lib/libavfilter-6.so \
        $$PWD/ffmpeg/lib/libavformat-57.so \
        $$PWD/ffmpeg/lib/libavutil-55.so \
        $$PWD/ffmpeg/lib/libpostproc-54.so \
        $$PWD/ffmpeg/lib/libswresample-2.so \
        $$PWD/ffmpeg/lib/libswscale-4.so


contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/ffmpeg/lib/libavcodec-57.so \
        $$PWD/ffmpeg/lib/libavdevice-57.so \
        $$PWD/ffmpeg/lib/libavfilter-6.so \
        $$PWD/ffmpeg/lib/libavformat-57.so \
        $$PWD/ffmpeg/lib/libavutil-55.so \
        $$PWD/ffmpeg/lib/libpostproc-54.so \
        $$PWD/ffmpeg/lib/libswresample-2.so \
        $$PWD/ffmpeg/lib/libswscale-4.so
}

}
