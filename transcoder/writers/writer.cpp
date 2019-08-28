#include "writer.h"
#include "aceswriter.h"
#include "hdrwriter.h"
#include "as02writer.h"
#include "proreswriter.h"
#include "ffmpegwriter.h"
#include "../utils/buffer.h"
#include <QStringList>

Writer::Writer(const WriterParams& s)
    :params_(s), error_("")
{
    setWriterInfo();
}

Writer::~Writer()
{

}

void Writer::setWriterInfo()
{

}

Writer* WriterFactory::createWriter(Writer::WriterType writer, const Writer::WriterParams &params)
{
    Writer* w = nullptr;
    switch (writer) {
    case Writer::AS02_MXF_Writer:
        w =  new AS02Writer(params);
        break;
    case Writer::ACES_MXF_Writer:
        w = new ACESWriter(params);
        break;
    case Writer::DOLBY_HDR_Writer :
        w = new HDRWriter(params);
        break;
    case Writer::PRORES_Writer:
        w = new ProResWriter(params);
        break;
    case Writer::FFMPEG_Writer:
        w = new FFMPEGWriter(params);
    }
    return w;
}
