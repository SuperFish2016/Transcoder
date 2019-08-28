#include "hdrwriter.h"
#include <QtDebug>
HDRWriter::HDRWriter(const WriterParams &p)
    :Writer(p)
{
    error_ = "";
}

HDRWriter::~HDRWriter()
{

}

bool HDRWriter::open()
{
    return true;
}

void HDRWriter::close()
{
}

bool HDRWriter::writeFrame(TSR::FrameBuffer* buffer)
{
    qDebug() << "Dolby HDR Writer frame: " << buffer->frameNumber();
    return true;
}
