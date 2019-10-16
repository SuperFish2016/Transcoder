#include "decoder.h"
#include "ffmpegdecoder.h"
#include "mxfdecoder.h"
#include "../utils/buffer.h"
#include "../utils/transcoder_common.h"
using namespace TSR;
Decoder::Decoder(TranscoderOption *options)
    :transOptions_(options)
{

}

Decoder::~Decoder()
{

}

Decoder* DecoderFactory::createDecoder(Decoder::DecoderType decoder, TranscoderOption* options)
{
    Decoder* d = nullptr;
    switch (decoder) {
    case Decoder::MXF_Decoder:
        d =  new MXFDecoder(options);
        break;
    case Decoder::FFmpeg_Decoder:
        d = new FFmpegDecoder(options);
        break;
    }
    return d;
}
