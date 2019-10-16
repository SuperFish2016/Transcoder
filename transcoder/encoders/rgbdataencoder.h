#ifndef RGBDATAENCODER_H
#define RGBDATAENCODER_H

#include "encoder.h"
class QImage;
class RGBDataEncoder : public Encoder
{
public:
    RGBDataEncoder(const EncoderParams& s);
    ~RGBDataEncoder(){}
    void open(){}
    void close(){}
    virtual QString encoderName() const{return "RGBDataEncoder";}
    virtual EncoderType encoderType() const {return Encoder::RGBData_Encoder;}
    virtual bool encodeFrame(TSR::FrameBuffer* src, TSR::FrameBuffer* dst);
private:
    struct SwsContext *sws_ctx;
};

#endif // RGBDATAENCODER_H
