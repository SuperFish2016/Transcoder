#include "encoder.h"
#include "ffmpegencoder.h"
#include "jpeg2000encoder.h"
#include "rgbdataencoder.h"
using namespace TSR;

Encoder::Encoder(const EncoderParams& s)
    :params(s)
{

}

QString Encoder::encoderName() const
{
    return "Encoder";
}

Encoder* EncoderFactory::createEncoder(Encoder::EncoderType encoder, const Encoder::EncoderParams &params)
{
    Encoder* e = nullptr;
    switch (encoder) {
    case Encoder::JPEG2000_Encoder:
        e =  new JPEG2000Encoder(params);
        break;
    case Encoder::FFMPEG_Encoder:
        e = new FFmpegEncoder(params);
        break;
    case Encoder::RGBData_Encoder:
        e = new RGBDataEncoder(params);
    }
    return e;
}



