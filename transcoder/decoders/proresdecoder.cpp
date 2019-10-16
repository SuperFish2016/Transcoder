#include "proresdecoder.h"
#include "../utils/buffer.h"
using namespace TSR;

ProResDecoder::ProResDecoder(TranscoderOption* option):
    Decoder(option)
{

}

FrameBuffer* ProResDecoder::decodeFrame(qint32 i)
{
    uchar data[10];
    FrameBuffer* buffer = new FFmpegBuffer();
    return buffer;
}
