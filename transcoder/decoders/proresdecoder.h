#ifndef PRORESDECODER_H
#define PRORESDECODER_H
#include <QString>
#include <QThread>
#include "decoder.h"
#include "../utils/videodefs.h"
struct TranscoderOption;
namespace TSR
{
    class FrameBuffer;
}

class ProResDecoder : public Decoder
{
public:
    ProResDecoder(TranscoderOption* option);
    ~ProResDecoder(){}
    virtual void closeDecoder(){}
    virtual QString decoderName() const{return "ProResDecoder";}
    TSR::FrameBuffer* decodeFrame(qint32 i);
};

#endif // PRORESDECODER_H
