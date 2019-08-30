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
    ReadThread(DecodedFramesQueue* queue, const QList<VideoSource>& videoList_, volatile bool *stopped_);
    void putOneFrame(FrameBuffer*);
signals:
    void reportStatus(TranscoderError stauts);
protected:
    void run();
private:
    QList<VideoSource>  videoList_;
    DecodedFramesQueue*     framesQueue_;
    volatile bool       *stopped_;
};

#endif // READER_H
