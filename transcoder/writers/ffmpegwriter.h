#ifndef PLAYERWRITER_H
#define PLAYERWRITER_H


#include "writer.h"
class FFMPEGWriter : public Writer
{
public:
    FFMPEGWriter(const WriterParams& s);
    ~FFMPEGWriter();

    virtual QString writerName() const{return "FFMPEGWriter";}
    virtual bool writeFrame(TSR::FrameBuffer* src);
private:
    bool open();
    void close();
private:

};
#endif // PLAYERWRITER_H
