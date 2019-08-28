#include "aceswriter.h"
#include <QtDebug>
ACESWriter::ACESWriter(const WriterParams &p)
    :Writer(p)
{
    error_ = "";
}

ACESWriter::~ACESWriter()
{

}

bool ACESWriter::open()
{
    return true;
}

void ACESWriter::close()
{

}

bool ACESWriter::writeFrame(TSR::FrameBuffer* buffer)
{
    return true;
}
