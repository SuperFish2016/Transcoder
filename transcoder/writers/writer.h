#ifndef WRITER_H
#define WRITER_H
#include <QString>

#include "../utils/buffer.h"
class Writer
{
public:
    enum WriterType
    {
        AS02_MXF_Writer = 0,
        ACES_MXF_Writer,
        DOLBY_HDR_Writer,
        PRORES_Writer,
        FFMPEG_Writer
    };

    struct WriterParams
    {
        quint32 bitDepth;
        QString outputFilename;
        QString metadataFilename;
        QString metadataOptions;
        QString outputColorSpace; // RGB 444
        qint32  rSize;
    };

public:
    Writer(const WriterParams& s);
    virtual ~Writer();
    QString error() const {return error_;}

    virtual bool open() = 0;  // open encoder
    virtual void close() = 0; // close encoder

    virtual QString writerName() const = 0;
    virtual bool writeFrame(TSR::FrameBuffer*) = 0;
private:
    void setWriterInfo();
protected:

    WriterParams params_;
    QString      error_;
};

class WriterFactory
{
public:
    static Writer* createWriter(Writer::WriterType writer, const Writer::WriterParams& params);
};

#endif
