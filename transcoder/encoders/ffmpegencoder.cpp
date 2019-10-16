#include "ffmpegencoder.h"
#include "../utils/buffer.h"
#include <QDebug>
#include <QImage>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}
FFmpegEncoder::FFmpegEncoder(const EncoderParams &p)
    :Encoder(p),sws_ctx(nullptr)
{
    qDebug() << "FFmpeg Encoder created.";
}

bool FFmpegEncoder::encodeFrame(TSR::FrameBuffer *srcBuffer, TSR::FrameBuffer *dstBuffer)
{
    TSR::FFmpegBuffer* src = dynamic_cast<TSR::FFmpegBuffer*>(srcBuffer);
    TSR::FFmpegBuffer* dst = dynamic_cast<TSR::FFmpegBuffer*>(dstBuffer);
    if(src->avFrame() != nullptr)
    {
        if(sws_ctx == nullptr)
            sws_ctx = sws_getContext(src->avFrameInfo().width, src->avFrameInfo().height,
                                     static_cast<AVPixelFormat>(src->avFrameInfo().pix_fmt),
                                     src->avFrameInfo().width, src->avFrameInfo().height,
                                     static_cast<AVPixelFormat>(AV_PIX_FMT_RGB24),
                                     SWS_BILINEAR, nullptr, nullptr, nullptr);
        AVFrame* pFrameRGB = av_frame_alloc();
        int ret = av_image_alloc(pFrameRGB->data, pFrameRGB->linesize, src->avFrameInfo().width, src->avFrameInfo().height, AV_PIX_FMT_RGB24, 16); // must freed with av_freep;
        if(pFrameRGB == nullptr)
            return false;

        if (ret < 0)
             fprintf(stderr, "Could not allocate source image\n");
        sws_scale(sws_ctx, (const uint8_t * const*)src->avFrame()->data, src->avFrame()->linesize, 0, src->avFrameInfo().height, pFrameRGB->data, pFrameRGB->linesize);
        qDebug() << "SRC frame: " << src->frameNumber();

        dst->setUserFreeFlag();
        dst->setAVFrame(pFrameRGB);
        dst->setAVFrameInfo(src->avFrameInfo());
        dst->setFrameNumber(src->frameNumber());
        delete srcBuffer;
        srcBuffer = nullptr;
    }
    return true;
}
