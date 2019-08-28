#ifndef FFMPEGDECODER_H
#define FFMPEGDECODER_H
#include <QString>
#include <QThread>
#include "decoder.h"
#include "../utils/videodefs.h"
class AVFormatContext;
class AVCodecContext;
class AVCodecParserContext;
class AVFrame;
class AVPacket;
namespace TSR
{
    class FrameBuffer;
}

typedef struct StreamContext
{
    AVCodecContext* dec_ctx;
    AVCodecParserContext* parser_ctx_;
}StreamContext;

class FFmpegDecoder : public Decoder
{
public:
    FFmpegDecoder(const VideoSource& s);
    ~FFmpegDecoder() override{}
    virtual void closeDecoder() override;
    virtual bool openDecoder() override;
    virtual QString decoderName() const override{return "FFmpeg";}
    TSR::FrameBuffer* decodeFrame(quint32 i) override;

    int frameCount;
private:
    bool openFile();

    AVFormatContext* input_fmt_ctx_; // 输入文件context
    AVCodecContext*  input_codec_ctx_;
    AVFrame*  input_frame_;
    AVPacket* input_packet_;
    StreamContext* stream_ctx_;
};


#endif // DECODER_H
