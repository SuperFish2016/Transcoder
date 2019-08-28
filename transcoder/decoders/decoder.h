#ifndef DECODER_H
#define DECODER_H
#include <QString>
#include <QThread>
#include "../utils/videodefs.h"
namespace TSR
{
    class FrameBuffer;
}

class Decoder
{
public:
    enum DecoderType
    {
        MXF_Decoder = 0,
        FFmpeg_Decoder
    };

public:
    Decoder(const VideoSource& s);
    virtual ~Decoder();
    /* Try to open decoder for input file, after create the Decoder,
     * then should call this to open input file decoder, decoder should return if open failed.
    */
    virtual bool openDecoder() = 0;
    virtual void closeDecoder() = 0;
    virtual QString decoderName() const = 0;
    virtual TSR::FrameBuffer* decodeFrame(quint32 i) = 0;

    qint32 frameCount(){return videoSource_.frameCount;}
protected:
    VideoSource  videoSource_;
};

class DecoderFactory
{
public:
    static Decoder* createDecoder(Decoder::DecoderType decoder, const VideoSource& source);
};

#endif // DECODER_H
