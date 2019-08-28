#ifndef PRORESDECODER_H
#define PRORESDECODER_H
#include <QString>
#include <QThread>
#include "decoder.h"
#include "../utils/videodefs.h"
namespace TSR
{
    class FrameBuffer;
}

class ProResDecoder : public Decoder
{
public:
    ProResDecoder(const VideoSource& s);
    ~ProResDecoder(){}
    virtual void closeDecoder(){}
    virtual QString decoderName() const{return "ProResDecoder";}
    TSR::FrameBuffer* decodeFrame(quint32 i);
};

#endif // PRORESDECODER_H
