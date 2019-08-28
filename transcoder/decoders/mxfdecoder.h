#ifndef MXFDECODER_H
#define MXFDECODER_H
#include <QString>
#include <QThread>
#include "decoder.h"
#include "../utils/videodefs.h"
namespace TSR
{
    class FrameBuffer;
}

class MXFDecoder : public Decoder
{
public:
    MXFDecoder(const VideoSource& s);
    ~MXFDecoder() override{}
    virtual bool openDecoder() override{ return  true;}
    virtual void closeDecoder() override{}
    virtual QString decoderName() const override{return "MXFDecoder";}
    TSR::FrameBuffer* decodeFrame(quint32 i) override;
};

#endif // DECODER_H
