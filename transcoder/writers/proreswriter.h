#ifndef PRORESWRITER_H
#define PRORESWRITER_H

#include "writer.h"
class ProResWriter : public Writer
{
public:
    ProResWriter(const WriterParams& s);
    ~ProResWriter();
    bool open();
    void close();
    virtual QString writerName() const{return "ProResWriter";}
    virtual bool writeFrame(TSR::FrameBuffer*);
private:
};


#endif // PRORESWRITER_H
