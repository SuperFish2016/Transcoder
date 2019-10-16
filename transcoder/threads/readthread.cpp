#include "readthread.h"
#include "../decoders/decoder.h"
#include <QDebug>
ReadThread::ReadThread(DecodedFramesQueue* queue, volatile bool *stopped, TranscoderOption* transOption)
    :framesQueue_(queue), stopped_(stopped), transOption_(transOption)
{

}

void ReadThread::putOneFrame(FrameBuffer *frame)
{
    framesQueue_->enqueue(frame);
}

void ReadThread::run()
{
    qDebug() << "ReadThread Thread > Started. ";
    QScopedPointer<Decoder> decoder ;
    qint32 frameIndex = 0;
    decoder.reset(DecoderFactory::createDecoder(Decoder::FFmpeg_Decoder, transOption_));
    if(!decoder->openDecoder())
    {
        emit reportStatus(TSR_DECODER_OPEN_ERROR);
        return;
    }
    for(qint32 i = transOption_->sourceInfo.startFrame; i <= transOption_->sourceInfo.endFrame; i++)
    {
        if(*stopped_)
        {
            break;
        }
        FrameBuffer* frame = decoder->decodeFrame(i + 1);
        if(frame == nullptr)
        {
            emit reportStatus(TSR_DECODER_ERROR);
            break;
        }
        frame->setFrameNumber(frameIndex++);
        putOneFrame(frame);
    }
    emit reportStatus(TSR_DECODER_END);
    qDebug() << "ReadThread Thread > END. ";
}
