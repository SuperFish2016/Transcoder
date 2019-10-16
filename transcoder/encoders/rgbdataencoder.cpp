#include "rgbdataencoder.h"
#include <QDebug>
#include "../utils/buffer.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}
RGBDataEncoder::RGBDataEncoder(const EncoderParams &p)
    :Encoder(p),sws_ctx(nullptr)
{
    qDebug() << "FFmpeg Encoder created.";
}

bool RGBDataEncoder::encodeFrame(TSR::FrameBuffer *srcBuffer, TSR::FrameBuffer *dstBuffer)
{
    TSR::FFmpegBuffer* src = dynamic_cast<TSR::FFmpegBuffer*>(srcBuffer);
    TSR::J2KBuffer* dst = dynamic_cast<TSR::J2KBuffer*>(dstBuffer);
    int dstWidth = src->avFrame()->width;
    int dstHeight = src->avFrame()->height;
    if(src->avFrame() != nullptr)
    {
        if(sws_ctx == nullptr)
            sws_ctx = sws_getContext(src->avFrameInfo().width, src->avFrameInfo().height, static_cast<AVPixelFormat>(src->avFrameInfo().pix_fmt),
                                                    dstWidth, dstHeight, static_cast<AVPixelFormat>(AV_PIX_FMT_RGB24), SWS_BILINEAR, nullptr, nullptr, nullptr);


        AVFrame* pFrameRGB = av_frame_alloc();
        int ret = av_image_alloc(pFrameRGB->data, pFrameRGB->linesize, dstWidth, dstHeight, AV_PIX_FMT_RGB24, 16); // must freed with av_freep;
        if(pFrameRGB == nullptr)
            return false;

        if (ret < 0)
        {
             fprintf(stderr, "Could not allocate source image\n");
        }
        sws_scale(sws_ctx, (const uint8_t * const*)src->avFrame()->data, src->avFrame()->linesize, 0, dstHeight, pFrameRGB->data, pFrameRGB->linesize);

        uchar* data = new uchar[pFrameRGB->linesize[0] * dstHeight];
        memcpy(data, pFrameRGB->data[0], pFrameRGB->linesize[0] * dstHeight);
        dst->setData(data, pFrameRGB->linesize[0] * dstHeight);
        dst->setFrameNumber(src->frameNumber());
        av_freep(&pFrameRGB->data[0]);
        delete srcBuffer;
        srcBuffer = nullptr;
    }
    return true;
}
