#ifndef READER_H
#define READER_H
#include <QThread>
#include <QMutex>
#include <QQueue>
#include "../utils/buffer.h"
#include "../transcoder.h"
#include "../utils/videodefs.h"
using namespace TSR;


class ReadThread : public QThread
{
    Q_OBJECT
public:
    ReadThread(DecodedFramesQueue* queue, volatile bool *stopped_, TranscoderOption* transOption);
    void putOneFrame(FrameBuffer*);
signals:
    void reportStatus(TranscoderError stauts);
protected:
    void run();
private:
    DecodedFramesQueue*  framesQueue_;
    volatile bool*       stopped_;
    TranscoderOption*    transOption_;
};

#endif // READER_H
