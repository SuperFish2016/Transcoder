#include "as02writer.h"
AS02Writer::AS02Writer(const WriterParams &p)
    :Writer(p)
{
    error_ = "";
}

AS02Writer::~AS02Writer()
{

}



bool AS02Writer::open()
{
    return true;
}

void AS02Writer::close()
{

}

bool AS02Writer::writeFrame(TSR::FrameBuffer* buffer)
{

    return true;
}
