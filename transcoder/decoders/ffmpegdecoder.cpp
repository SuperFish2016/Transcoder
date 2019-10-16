#include "ffmpegdecoder.h"
#include "../utils/buffer.h"
#include <QDebug>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
}

using namespace TSR;


FFmpegDecoder::FFmpegDecoder(TranscoderOption* option):
    Decoder(option), input_frame_(nullptr), input_packet_(nullptr)
{
    qDebug() << "FFmpeg Decoder created.";
}

FFmpegDecoder::~FFmpegDecoder()
{
    qDebug() << "FFmpeg Decoder freed.";
    closeDecoder();
}

bool FFmpegDecoder::openDecoder()
{
    bool ret = openFile();
    if(ret == 0)
        return true;
    return false;
}

void FFmpegDecoder::closeDecoder()
{
    av_frame_free(&input_frame_);
    av_packet_free(&input_packet_);
    avcodec_free_context(&input_codec_ctx_);
    avformat_close_input(&input_fmt_ctx_);
}

QString getDurationTime(long long duration)
{
    if(duration != AV_NOPTS_VALUE){
       int hours,mins,secs,us;
       int64_t d = duration + 5000;
       secs = d /AV_TIME_BASE;
       us= d % AV_TIME_BASE;//1000000
       mins = secs/60;
       secs %= 60;
       hours = mins/60;
       mins %= 60;
       return QString("%1:%2:%3:%4").arg(hours, 2, 10, QLatin1Char('0')).arg(mins, 2, 10, QLatin1Char('0')).arg(secs, 2, 10, QLatin1Char('0')).arg(100*us/AV_TIME_BASE);
    }
    return "00:00:00:00";
}

qint32 getDurationFrameCount(long long duration, const AVRational& frameRate)
{
    qint32 frameCount = 0;
    int hours,mins,secs,us;
    if(duration != AV_NOPTS_VALUE){
       int64_t d = duration + 5000;
       secs = d /AV_TIME_BASE;
       us= d % AV_TIME_BASE;//1000000
       mins = secs/60;
       secs %= 60;
       hours = mins/60;
       mins %= 60;
    }
    frameCount = secs * frameRate.num / frameRate.den;
}

bool FFmpegDecoder::openFile()
{
    input_fmt_ctx_ = nullptr;
    int ret;
    if((ret = avformat_open_input(&input_fmt_ctx_, transOptions_->sourceInfo.filename.c_str(),nullptr, nullptr)) < 0){
        qDebug() << "Cannot open input file, error code: " << ret; return ret;
    }
    if((ret = avformat_find_stream_info(input_fmt_ctx_, nullptr)) < 0){
        qDebug() << "Cannot find stream information. error code: " << ret; return ret;
    }


    stream_ctx_ = static_cast<StreamContext*>(av_mallocz_array(input_fmt_ctx_->nb_streams, sizeof(*stream_ctx_)));
    for(uint i = 0; i < input_fmt_ctx_->nb_streams; ++i)
    {
        AVStream* stream = input_fmt_ctx_->streams[i];
        AVCodec* dec_codec;
        qDebug() << "File name:" << transOptions_->sourceInfo.filename.c_str();
        qDebug() << "Codec Id: " << stream->codecpar->codec_id;
        qDebug() << "format name:" << input_fmt_ctx_->iformat->long_name;

        /* = 的优先级 很低，要低于比较运算符 */
        if(!(dec_codec = avcodec_find_decoder(stream->codecpar->codec_id))){
            qDebug() << "Failed to find decoder for stream " << i; return -1;
        }

        stream_ctx_[i].parser_ctx_ = av_parser_init(dec_codec->id);
        if(!stream_ctx_[i].parser_ctx_)
        {
            qDebug() << "Parser not found.";
        }
        if(!(input_codec_ctx_ = avcodec_alloc_context3(dec_codec))){
            qDebug() << "Failed to allocate the decoder context for stream " << i; return -1;
        }
        if((ret = avcodec_parameters_to_context(input_codec_ctx_, stream->codecpar)) < 0){
            qDebug() << "Failed to copy decoder parameters to input decoder context for stream " << i; return ret;
        }
        if(input_codec_ctx_->codec_type == AVMEDIA_TYPE_VIDEO )//|| input_codec_ctx_->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            if(input_codec_ctx_->codec_type == AVMEDIA_TYPE_VIDEO)
                input_codec_ctx_->framerate = av_guess_frame_rate(input_fmt_ctx_, stream, nullptr);
            qDebug() << "Video frame rate: " << QString("%1 / %2").arg(input_codec_ctx_->framerate.num).arg(input_codec_ctx_->framerate.den);
            if((ret = avcodec_open2(input_codec_ctx_, dec_codec, nullptr)) < 0){
                qDebug() << "Failed to open decoder for stream " << i; return ret;
            }
            qDebug() << " codec ctx width & height: " << input_codec_ctx_->width << input_codec_ctx_->height;
        }
        av_dump_format(input_fmt_ctx_, 0, transOptions_->sourceInfo.filename.c_str(), 0);
        break;
    }
    qDebug() << getDurationTime(input_fmt_ctx_->duration);
   // videoSource_.frameCount = getDurationFrameCount(input_fmt_ctx_->duration, input_codec_ctx_->framerate);


    input_frame_ = av_frame_alloc();
    if (!input_frame_) {
        qDebug() << "Could not allocate video frame\n";
        return -1;
    }

    input_packet_ = av_packet_alloc();
    if(!input_packet_)
    {
        qDebug() << "Failed to alloc packet.";
        return -1;
    }

    qDebug() << "File open Successfully.";
    return 0;
}


FrameBuffer* FFmpegDecoder::decodeFrame(qint32 i)
{

    int frameFinished;
    FFmpegBuffer* buffer = nullptr;
    input_codec_ctx_->refcounted_frames = 1;
    while(av_read_frame(input_fmt_ctx_, input_packet_) >= 0)
    {

        if(input_packet_->stream_index == 0)
        {
            AVFrame* out_frame = av_frame_alloc();
            avcodec_decode_video2(input_codec_ctx_,  out_frame, &frameFinished, input_packet_);
            if(frameFinished)
            {
                if(input_codec_ctx_->frame_number  < static_cast<int>(i))
                {
                    continue;
                }
                else if(input_codec_ctx_->frame_number == static_cast<int>(i))
                {
                    qDebug() << " Sucessfully decode frame number: " << input_codec_ctx_->frame_number << "pts: " << input_frame_->pts;
                    buffer = new FFmpegBuffer();
                    FFmpegBuffer::FrameInfo buffInfo;
                    buffInfo.width = input_codec_ctx_->width;
                    buffInfo.height = input_codec_ctx_->height;
                    buffInfo.pix_fmt = input_codec_ctx_->pix_fmt;
                    buffer->setAVFrame(out_frame);
                    buffer->setAVFrameInfo(buffInfo);
                    break;
                }
                else
                {
                    qDebug() << " decode frame number: " << i << " failed. current codec_ctx_->frame_number: " << input_codec_ctx_->frame_number;
                    //break;
                }
            }
        }

    }
    return buffer;
}
