#ifndef HDRWRITER_H
#define HDRWRITER_H
#include "writer.h"
class HDRWriter : public Writer
{
public:
    HDRWriter(const WriterParams& s);
    ~HDRWriter();
    bool open();
    void close();
    virtual QString writerName() const{return "HDRWriter";}
    virtual bool writeFrame(TSR::FrameBuffer*);
};

#endif // HDRWRITER_H
