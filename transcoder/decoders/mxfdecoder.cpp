#include "mxfdecoder.h"
#include "../utils/buffer.h"
#include <QDebug>
using namespace TSR;

MXFDecoder::MXFDecoder(const VideoSource& s )
    :Decoder(s)
{
    qDebug() << "FFmpeg Encoder created.";
}

FrameBuffer* MXFDecoder::decodeFrame(quint32 i)
{
    uchar *data = new uchar[10];
    J2KBuffer* buffer = new J2KBuffer(data, 10, i);
    return buffer;
}

