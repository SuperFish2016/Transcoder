#include "mxfdecoder.h"
#include "../utils/buffer.h"
#include <QDebug>
using namespace TSR;

MXFDecoder::MXFDecoder(TranscoderOption* option )
    :Decoder(option)
{
    qDebug() << "FFmpeg Encoder created.";
}

FrameBuffer* MXFDecoder::decodeFrame(qint32 i)
{
    uchar *data = new uchar[10];
    J2KBuffer* buffer = new J2KBuffer(data, 10, i);
    return buffer;
}

