#ifndef BUFFER_H
#define BUFFER_H
#include <QString>
/*Undefined symbols for architecture x86_64 Error: Need check if some class constructor implement.*/
struct AVFrame;
struct AVCodecContext;
namespace TSR {

class FrameBuffer
{
public:
    enum BufferType{J2KBuffer, FFmpegBuffer};
    FrameBuffer();
    FrameBuffer(qint32 encodeId, qint32 frameNo);
    virtual ~FrameBuffer();
    virtual BufferType bufferType() const = 0;

    void setFrameNumber(qint32 frameIndex){frame_number_ = frameIndex;}
    inline qint32 frameNumber() const{return frame_number_;}

    inline void setEncoderId(qint32 id){encoder_id = id;}
    qint32 encoderId() const{return encoder_id;}

protected:
    qint32 encoder_id; // 标记是哪个编码线程对器进行编码，主要用于同步
    qint32 frame_number_;  // 该帧的帧号
};

class J2KBuffer : public FrameBuffer
{
public:
    J2KBuffer();
    J2KBuffer(uchar* data, qint32 size, qint32 frameNo);
    ~J2KBuffer() override;
    virtual BufferType bufferType() const override{return FrameBuffer::J2KBuffer;}

    void setData(uchar *data, qint32 data_size);
    inline uchar* data(){return data_;}
    inline uchar* constData(){return data_;}

    inline qint32 size() const{return data_size_;}
    inline void setSize(qint32 size){data_size_ = size;}
private:
    uchar*  data_;
    qint32 data_size_;
};

class FFmpegBuffer : public FrameBuffer
{
public:
    struct FrameInfo
    {
        int width;
        int height;
        int pix_fmt;
    };
public:
    FFmpegBuffer();
    ~FFmpegBuffer() override;
    virtual BufferType bufferType() const override{return FrameBuffer::FFmpegBuffer;}

    AVFrame* avFrame();
    void setAVFrame(AVFrame* f);

    /* add this frameInfo with simple info instead of AVContextCodec.
    *  if pass context ctx, after decoder end, the context ctx will be freed. It will caused a crash.
    */
    const FrameInfo& avFrameInfo();
    void setAVFrameInfo(const FrameInfo&);

    void setUserFreeFlag(){needBeFreedByUser_ = true;}
private:
    AVFrame* av_frame_;
    FrameInfo av_frame_info_;
    bool needBeFreedByUser_;
};
}



#endif // BUFFER_H
