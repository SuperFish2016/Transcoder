#include "readthread.h"
#include "../decoders/decoder.h"
#include <QDebug>
ReadThread::ReadThread(DecodedFramesQueue* queue, const QList<VideoSource>& videolist, volatile bool *stopped)
    :videoList_(videolist), framesQueue_(queue), stopped_(stopped)
{

}

void ReadThread::putOneFrame(FrameBuffer *frame)
{
    framesQueue_->enqueue(frame);
}

void ReadThread::run()
{
    qDebug() << "ReadThread Thread > Started. ";
    QListIterator<VideoSource> iter(videoList_);
    while(iter.hasNext())
    {
        VideoSource video = iter.next();
        //video.fileName = "/Users/chentao/Desktop/HPCA_BRCM_C.264";
        //video.fileName = "/Users/chentao/Desktop/pp.MPG";
        Decoder* decoder = nullptr;
        if(*stopped_)
            break;
        bool hasError = false;
        //if(video.fileName.endsWith("264"))
        {
            qint32 frameIndex = 0;
            decoder = DecoderFactory::createDecoder(Decoder::FFmpeg_Decoder, video);
            if(!decoder->openDecoder())
                break;
            qDebug() << "Decoder Open Successfully.";
            video.duration = decoder->frameCount();
            for(quint32 i = video.entryPoint; i < video.duration + video.entryPoint; i++)
            {
                if(*stopped_)
                {
                    hasError = true; // just for break inner loop.
                    break;
                }
                FrameBuffer* frame = decoder->decodeFrame(i + 1);
                if(frame == nullptr)
                {
                   hasError = true;
                   emit reportStatus(TSR_DECODER_ERROR);
                   break;
                }
                frame->setFrameNumber(frameIndex++);
                putOneFrame(frame);
            }
        }
        if(hasError)
            break;
    }
    emit reportStatus(TSR_DECODER_END);
    qDebug() << "ReadThread Thread > END. ";
}
