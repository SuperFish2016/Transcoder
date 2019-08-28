#include "writethread.h"
#include <QDebug>
#include <QTime>
#include "../writers/writer.h"
#include "../utils/buffer.h"
#include <QImage>
extern "C"
{
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
}

QString msToStandardTime(int ms)
{
    int seconds = 1000;
    int minutes = 60 * seconds;
    int hour = 60 * minutes;

    int h = ms / hour;
    int m = (ms - hour * h)/minutes;
    int s = (ms - hour * h - minutes * m) / seconds;

    return QString("%1:%2:%3").
            arg(h, 2, 10, QLatin1Char('0')).
            arg(m, 2, 10, QLatin1Char('0')).
            arg(s, 2, 10, QLatin1Char('0'));
}

void WriteThread::run()
{
    timer_.start();
    if(!createWriter())
    {
        emit reportStatus(WriterOpenError);
        return;
    }


    TSR::FrameBuffer* buffer = nullptr;
    for(int i = 0; i < frameCount_; i++)
    {
        if(*stopped_)
            break;
        forever
        {
            if(*stopped_)
                break;
            buffer = j2kFrameVector_->takeOne(i);
            if(buffer != nullptr)
                break;
        }
        if(*stopped_)
            break;
       writer_->writeFrame(buffer);

       TSR::FFmpegBuffer* ffmpegBuffer = dynamic_cast<TSR::FFmpegBuffer*>(buffer);
       if(ffmpegBuffer != nullptr)
       {
           QImage frameImage(ffmpegBuffer->avFrame()->data[0], ffmpegBuffer->avFrame()->linesize[0] / 3, ffmpegBuffer->avFrameCtx()->height, ffmpegBuffer->avFrame()->linesize[0] , QImage::Format_RGB888);
           emit qImageReady(frameImage.copy());
       }

        // do the write buffer data into file.
        delete buffer;
        updateProgressBar(i, timer_.elapsed());
    }

    emit progress(frameCount_);
}

void WriteThread::updateProgressBar(int currentFrame, int timeElapsed)
{
    emit progress(currentFrame + 1);
    if(currentFrame % 40 == 0)
    {
        if(timeElapsed < 0.00001) timeElapsed = 10;
        int fps = currentFrame * 1000.0 / timeElapsed;
        if(fps < 1) fps = 1;
        emit progressText(tr("Encoding \"%1\" \nElapsed Time: %2  Estimated Remaining: %3").
                          arg(fileName_).arg(msToStandardTime(timeElapsed)).arg(msToStandardTime((frameCount_ - currentFrame) * 1000/ fps)));
    }
}

bool WriteThread::createWriter()
{
    // create Writer Here
    Writer::WriterParams p;
    writer_ = WriterFactory::createWriter(Writer::FFMPEG_Writer, p);
    if(!writer_->open())
    {
        return false;
    }
    return true;
}
