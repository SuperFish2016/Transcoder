#ifndef AS02WRITER_H
#define AS02WRITER_H
#include "writer.h"
class AS02Writer : public Writer
{
public:
    AS02Writer(const WriterParams& s);
    ~AS02Writer();

    virtual QString writerName() const{return "AS02Writer";}
    virtual bool writeFrame(TSR::FrameBuffer* src);
private:
    bool open();
    void close();
private:
    
};


#endif // AS02WRITER_H
