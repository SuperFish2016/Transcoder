#include "jpeg2000encoder.h"
#include "../utils/buffer.h"
#include <QDebug>
extern "C"
{
#include <libavutil/frame.h>
}
JPEG2000Encoder::JPEG2000Encoder(const EncoderParams &p)
    :Encoder(p)
{
    qDebug() << "JPEG2000 Encoder created.";
}

bool JPEG2000Encoder::encodeFrame(TSR::FrameBuffer* src, TSR::FrameBuffer* dst)
{
    TSR::J2KBuffer* dstJ2KBuffer = dynamic_cast<TSR::J2KBuffer*>(dst);
    Q_ASSERT(dstJ2KBuffer != nullptr);
    TSR::FFmpegBuffer* srcFFmpegBuffer = dynamic_cast<TSR::FFmpegBuffer*>(src);
    TSR::J2KBuffer* srcJ2CBuffer = dynamic_cast<TSR::J2KBuffer*>(src);

    if(srcFFmpegBuffer != nullptr)
    {
        if(srcFFmpegBuffer->avFrame() != nullptr)
        {
            AVFrame* frame = srcFFmpegBuffer->avFrame();
        }
        dst->setFrameNumber(srcFFmpegBuffer->frameNumber());
    }
    return true;
}
