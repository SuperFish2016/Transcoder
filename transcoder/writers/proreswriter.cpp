#include "proreswriter.h"

ProResWriter::ProResWriter(const WriterParams &p)
    :Writer(p)
{
    error_ = "";
}

ProResWriter::~ProResWriter()
{

}

bool ProResWriter::open()
{
    return true;
}

void ProResWriter::close()
{

}

bool ProResWriter::writeFrame(TSR::FrameBuffer* buffer)
{
    return true;
}
