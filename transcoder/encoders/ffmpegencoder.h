#ifndef FFMPEGENCODER_H
#define FFMPEGENCODER_H

#include "encoder.h"
class QImage;
class FFmpegEncoder : public Encoder
{
public:
    FFmpegEncoder(const EncoderParams& s);
    ~FFmpegEncoder(){}
    void open(){}
    void close(){}
    virtual QString encoderName() const{return "FFmpegEncoder";}
    virtual EncoderType encoderType() const {return Encoder::FFMPEG_Encoder;}
    virtual bool encodeFrame(TSR::FrameBuffer* src, TSR::FrameBuffer* dst);
private:
    struct SwsContext *sws_ctx;
};


#endif // FFMPEGENCODER_H
