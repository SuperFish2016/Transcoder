#ifndef ENCODER_H
#define ENCODER_H
#include <QString>
namespace TSR{
class FrameBuffer;
};
class Encoder
{
public:
    enum EncoderType
    {
        JPEG2000_Encoder = 0,
        FFMPEG_Encoder
    };

    struct EncoderParams
    {
        qint32 bitDepth;
        qint32 width;
        qint32 height;
        qint32 avPixelFormat;
    };

public:
    Encoder(const EncoderParams& s);
    virtual ~Encoder(){}
    virtual void open() = 0;  // open encoder
    virtual void close() = 0; // close encoder
    virtual QString encoderName() const;
    virtual EncoderType encoderType() const = 0;
    /* encode a src frame -> j2k frame */
    virtual bool encodeFrame(TSR::FrameBuffer* src, TSR::FrameBuffer* dst) = 0;
protected:
    EncoderParams params;
};




class EncoderFactory
{
public:
    static Encoder* createEncoder(Encoder::EncoderType encoder, const Encoder::EncoderParams& source);
};

#endif
