#ifndef ACESWRITER_H
#define ACESWRITER_H
#include "writer.h"
class ACESWriter : public Writer
{
public:
    ACESWriter(const WriterParams& s);
    ~ACESWriter();
    bool open();
    void close();
    virtual QString writerName() const{return "ACESWriter";}
    virtual bool writeFrame(TSR::FrameBuffer*);
};

#endif // ACESWRITER_H
