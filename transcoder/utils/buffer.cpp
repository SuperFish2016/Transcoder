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
    :av_frame_(nullptr),av_frame_ctx_(nullptr),needBeFreedByUser_(false)
{

}

TSR::FFmpegBuffer::~FFmpegBuffer()
{
    if(needBeFreedByUser_)
        av_freep(&this->avFrame()->data[0]);
    av_frame_unref(av_frame_);
    qDebug() << "ffmpegbuffer :unref frame number -> " << this->frame_number_;
}

AVFrame* TSR::FFmpegBuffer::avFrame()
{
    return av_frame_;
}

void TSR::FFmpegBuffer::setAVFrame(struct AVFrame *f)
{
    av_frame_ = f;
}

const TSR::FFmpegBuffer::FrameInfo& TSR::FFmpegBuffer::avFrameInfo()
{
    return av_frame_info_;
}

void TSR::FFmpegBuffer::setAVFrameInfo(const FrameInfo &f)
{
    av_frame_info_ = f;
}
