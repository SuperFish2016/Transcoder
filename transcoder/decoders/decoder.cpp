#include "decoder.h"
#include "ffmpegdecoder.h"
#include "mxfdecoder.h"
#include "../utils/buffer.h"
using namespace TSR;
Decoder::Decoder(const VideoSource& s)
    :videoSource_(s)
{

}

Decoder::~Decoder()
{

}

Decoder* DecoderFactory::createDecoder(Decoder::DecoderType decoder, const VideoSource& source)
{
    Decoder* d = nullptr;
    switch (decoder) {
    case Decoder::MXF_Decoder:
        d =  new MXFDecoder(source);
        break;
    case Decoder::FFmpeg_Decoder:
        d = new FFmpegDecoder(source);
        break;
    }
    return d;
}
