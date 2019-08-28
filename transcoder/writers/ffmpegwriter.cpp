#include "ffmpegwriter.h"

FFMPEGWriter::FFMPEGWriter(const WriterParams &p)
    :Writer(p)
{
    error_ = "";
}

FFMPEGWriter::~FFMPEGWriter()
{

}



bool FFMPEGWriter::open()
{
    return true;
}

void FFMPEGWriter::close()
{

}

bool FFMPEGWriter::writeFrame(TSR::FrameBuffer* buffer)
{
    return true;
}
