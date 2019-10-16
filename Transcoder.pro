QT += core gui widgets

TARGET = Transcoder
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

QMAKE_CXXFLAGS += --std=c++0x

QMAKE_LFLAGS += -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -DNOGDI

#add ffmpeg lib
INCLUDEPATH += $$PWD/ffmpeg/include
LIBS += -L$$PWD/ffmpeg/lib -lavcodec -lavformat -lavfilter -lswscale -lavutil -lswresample

SOURCES += main.cpp \
    transcoder/decoders/decoder.cpp \
    transcoder/utils/buffer.cpp \
    transcoder/transcoder.cpp\
    transcoder/threads/writethread.cpp \
    transcoder/threads/readthread.cpp \
    transcoder/threads/encodethread.cpp \
    transcoder/encoders/encoder.cpp \
    transcoder/writers/writer.cpp \
    transcoder/decoders/ffmpegdecoder.cpp \
    transcoder/decoders/mxfdecoder.cpp \
    transcoder/decoders/proresdecoder.cpp \
    transcoder/writers/hdrwriter.cpp \
    transcoder/writers/aceswriter.cpp \
    transcoder/writers/as02writer.cpp \
    transcoder/writers/proreswriter.cpp \
    transcoder/utils/transcoder_common.cpp \
    transcoder/encoders/ffmpegencoder.cpp \
    transcoder/encoders/jpeg2000encoder.cpp \
    transcoder/writers/ffmpegwriter.cpp \
    progresswidget.cpp \
    transcoder/encoders/rgbdataencoder.cpp
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    transcoder/decoders/decoder.h \
    transcoder/utils/buffer.h \
    transcoder/transcoder.h \
    transcoder/utils/videodefs.h \
    transcoder/threads/threadsafequeue.h \
    transcoder/threads/threadsafevector.h \
    transcoder/threads/writethread.h \
    transcoder/threads/readthread.h \
    transcoder/threads/encodethread.h \
    transcoder/encoders/encoder.h \
    transcoder/writers/writer.h \
    transcoder/decoders/ffmpegdecoder.h \
    transcoder/decoders/mxfdecoder.h \
    transcoder/decoders/proresdecoder.h \
    transcoder/writers/hdrwriter.h \
    transcoder/writers/aceswriter.h \
    transcoder/writers/as02writer.h \
    transcoder/writers/proreswriter.h \
    transcoder/utils/transcoder_common.h \
    transcoder/encoders/ffmpegencoder.h \
    transcoder/encoders/jpeg2000encoder.h \
    transcoder/writers/ffmpegwriter.h \
    progresswidget.h \
    transcoder/encoders/rgbdataencoder.h
SOURCES += \
        mainwindow.cpp \

HEADERS += \
        mainwindow.h \

FORMS += \
        mainwindow.ui \
    progresswidget.ui

RESOURCES += \
    res.qrc
