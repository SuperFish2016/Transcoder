#include "encodethread.h"
#include "../encoders/encoder.h"

EncodeThread::EncodeThread(DecodedFramesQueue *decodedFramesQueue, EncodedFramesVector *encodedframesVector, quint32 maxBufferSize, volatile bool *stopped)
    : DecodedFramesQueue_(decodedFramesQueue), encodedFramesVector_(encodedframesVector),
      maxBufferSize_(maxBufferSize), id_(0), stopped_(stopped)
 {
    Encoder::EncoderParams params;
    encoder_ = EncoderFactory::createEncoder(Encoder::FFMPEG_Encoder, params);
 }
TSR::FrameBuffer* EncodeThread::takeOneFrame()
{
    return DecodedFramesQueue_->dequeue();
}

void EncodeThread::run()
{
    qDebug() << "Encode Thread Started. ";
    encoder_->open();
    while(1)
    {
        if(*stopped_)
            break;
        TSR::FrameBuffer* buffer = takeOneFrame(); // buffer is NULL only deocde thread finsied.
        if(buffer == NULL)
            break;
        if(*stopped_)
            break;
        //int r  = qrand() % 10;;
        msleep(100);
        buffer->setEncoderId(id_);
        // make sure encode thread push data less than maxBufferSize;
        // eg. buffer size 64 ,thread number 5, so the thread maxBuffrSize can not more than 64 / 5;
        while(encodedFramesVector_->threadBufferCount(id_) >= maxBufferSize_ && !(*stopped_));

        TSR::FrameBuffer* resultBuffer = nullptr;
        if(encoder_->encoderType() == Encoder::FFMPEG_Encoder)
        {
            resultBuffer = new TSR::FFmpegBuffer();
        }
        else {
            resultBuffer = new TSR::J2KBuffer();
        }
        if(!encoder_->encodeFrame(buffer, resultBuffer))
        {
            emit reportStatus(TSR_ENCODER_ERROR);
            break;
        }
        encodedFramesVector_->pushData(resultBuffer);
    }
    encoder_->close();
    qDebug() << "Encode Thread " << id_ <<" > END. ";
}
