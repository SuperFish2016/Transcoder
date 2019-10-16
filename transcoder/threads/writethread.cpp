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
        emit reportStatus(TSR_WRITER_OPEN_ERROR);
        return;
    }

    if(!writer_->open())
        return;
    TSR::FrameBuffer* buffer = nullptr;
    for(int i = 0; i < frameCount_; i++)
    {
        if(*stopped_)
            break;
        forever
        {
            if(*stopped_)
                break;
            buffer = encodedFramesVector_->takeOne(i);
            if(buffer != nullptr)
                break;
        }
        if(*stopped_)
            break;
       writer_->writeFrame(buffer);

       TSR::J2KBuffer* j2kBuffer = dynamic_cast<TSR::J2KBuffer*>(buffer);
       TSR::FFmpegBuffer* ffmpegBuffer = dynamic_cast<TSR::FFmpegBuffer*>(buffer);
       if(j2kBuffer != nullptr)
       {
           if(j2kBuffer->data() != nullptr)
           {
               QImage frameImage(j2kBuffer->data(), 800, 640, 800 *3 , QImage::Format_RGB888);
               emit qImageReady(frameImage.copy());
           }
       }
       if(ffmpegBuffer != nullptr)
       {
           if(ffmpegBuffer->avFrame() != nullptr)
           {
               QImage frameImage(ffmpegBuffer->avFrame()->data[0], ffmpegBuffer->avFrameInfo().width, ffmpegBuffer->avFrameInfo().height,
                       ffmpegBuffer->avFrame()->linesize[0] , QImage::Format_RGB888);
               emit qImageReady(frameImage.copy());
           }
       }

        // do the write buffer data into file.
        delete buffer;
        updateProgressText(i, timer_.elapsed());
    }
    emit progress(frameCount_);
}

void WriteThread::updateProgressText(int currentFrame, int timeElapsed)
{
    emit progress(currentFrame + 1);
    if(currentFrame % 40 == 0)
    {
        if(timeElapsed < 0.00001) timeElapsed = 10;
        int fps = currentFrame * 1000.0 / timeElapsed;
        if(fps < 1) fps = 1;
        emit progressText(QString("Elapsed Time: %1").arg(msToStandardTime(timeElapsed)),
                          QString("Estimated Remaining: %1 , (fps %2)").arg(msToStandardTime((frameCount_ - currentFrame) * 1000/ fps)).arg(fps));
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
