#include "proresdecoder.h"
#include "../utils/buffer.h"
using namespace TSR;

ProResDecoder::ProResDecoder(const VideoSource& s):
    Decoder(s)
{

}

FrameBuffer* ProResDecoder::decodeFrame(quint32 i)
{
    uchar data[10];
    FrameBuffer* buffer = new FFmpegBuffer();
    return buffer;
}
