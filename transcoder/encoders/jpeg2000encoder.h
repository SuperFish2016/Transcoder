#ifndef JPEG2000ENCODER_H
#define JPEG2000ENCODER_H
#include "encoder.h"

class JPEG2000Encoder : public Encoder
{
public:
    JPEG2000Encoder(const EncoderParams& s);
    ~JPEG2000Encoder(){}
    void open(){}
    void close(){}
    virtual QString encoderName() const{return "JPEG2000Encoder";}
    virtual EncoderType encoderType() const {return Encoder::JPEG2000_Encoder;}
    virtual bool encodeFrame(TSR::FrameBuffer* src, TSR::FrameBuffer* dst);
};

#endif // JPEG2000ENCODER_H
