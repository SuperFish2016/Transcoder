#include "buffer.h"
#include <QDebug>
extern "C"
{
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>

}
TSR::FrameBuffer::FrameBuffer()
    :encoder_id(0),frame_number_(0)
{

}

TSR::FrameBuffer::FrameBuffer(qint32 id, qint32 no)
    :encoder_id(id),frame_number_(no)
{

}

TSR::FrameBuffer::~FrameBuffer()
{

}

TSR::J2KBuffer::J2KBuffer()
    : FrameBuffer(), data_(nullptr), data_size_(0)
{
}

TSR::J2KBuffer::J2KBuffer(uchar* data, qint32 size, qint32 frameNo)
    : FrameBuffer(0, frameNo), data_(data), data_size_(size)
{
}

TSR::J2KBuffer::~J2KBuffer()
{
    if(data_ != nullptr)
        delete[] data_;
    data_ = nullptr;
}

void TSR::J2KBuffer::setData(uchar *data, qint32 data_size)
{
    if(data == nullptr) return;
    if(data_ != nullptr)
        delete []data_;
    data_ = data;
    data_size_ = data_size;
}

TSR::FFmpegBuffer::FFmpegBuffer()
{

}

TSR::FFmpegBuffer::~FFmpegBuffer()
{
    av_frame_free(&av_frame_);
   // avcodec_free_context(&av_frame_ctx_);
}

AVFrame* TSR::FFmpegBuffer::avFrame()
{
    return av_frame_;
}

void TSR::FFmpegBuffer::setAVFrame(struct AVFrame *f)
{
    av_frame_ = f;
}

AVCodecContext* TSR::FFmpegBuffer::avFrameCtx()
{
    return av_frame_ctx_;
}

void TSR::FFmpegBuffer::setAVCodecCtx(AVCodecContext *ctx)
{
    av_frame_ctx_ = ctx;
}
